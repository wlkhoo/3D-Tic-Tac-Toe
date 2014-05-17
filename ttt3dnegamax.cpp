/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		ttt3dnegamax.cpp
	CLASS:		TTT3DNegamax
	DETAILS:	Negamax.
			Inherits from QThread.
*/
#include "ttt3dnegamax.h"

/*
	Constructor
	Initialize all variables to default value.
	Actual game is stored in m_boardArray.
*/
TTT3DNegamax::TTT3DNegamax(QObject *p)
	: QThread(p)
{
	m_unoccupiedSq	= 27;
	m_currentPlayer	= 1;

	m_boardArray	= new QVector<int>(27);
	m_undoStack		= new QStack<int>();

	for (int i = 0; i < 27; i++)
		m_boardArray	->insert(i, BlankSq);
}

/*
	Destructor
	Terminate current thread, safely
*/
TTT3DNegamax::~TTT3DNegamax()
{
	mutex.lock();
	cond.wakeOne();
	mutex.unlock();
	wait();
}

/*
	Reset all variables to default value.
*/
void TTT3DNegamax::reset()
{
	m_unoccupiedSq	= 27;
	m_currentPlayer	= 1;

	m_undoStack		->clear();
	for (int i = 0; i < 27; i++)
		m_boardArray	->replace(i, BlankSq);
}

/*
	Make a move on the board (m_boardArray)
	If it is called from Negamax function, this make move is virtual
		and will be restored at the end of the function.
	Once Negamax has been applied, a real makeMove will be called before
		returning to the main program; this move will be displayed.
	Once a move has been made, switch player.
*/
void TTT3DNegamax::makeMove(int pos, int side)
{
	m_boardArray	->replace(pos, side);
	m_unoccupiedSq--;
	m_undoStack	->push(pos);	// for restoration purpose
	m_currentPlayer ^= 0x3;		// Changing player (1->2; 2->1); bitwise exclusive or with 0x3.
}

/*
	Similar to makeMove, but this is undoMove
	This is only called from within Negamax function.
*/
void TTT3DNegamax::undoMove()
{
	m_boardArray	->replace(m_undoStack->top(), BlankSq);
	m_undoStack	->pop();
	m_unoccupiedSq++;
	m_currentPlayer ^= 0x3;
}

/*
	Preparing this thread (called from gameboard)
	Starting this thread with high priority.
	Setting which side the computer is on.
*/
void TTT3DNegamax::computerMove(int comp)
{
	QMutexLocker locker(&mutex);

	m_computer	= comp;

	if(!isRunning())
		start(HighPriority);
	else
		cond.wakeOne();
}

/*
	Call getResult() to check for victory.
	emit the result to gameboard.
*/
bool TTT3DNegamax::checkVictory()
{
	int result = getResult();
	if (result != 0)
	{
		if (result == 1)	// Player 1 wins
			emit victory(1);
		else if (result == 2)	// Player 2 wins
			emit victory(2);
		else			// Draw
			emit victory(3);
		return true;
	}
	return false;
}

/*
	Return current player.
*/
int TTT3DNegamax::currentPlayer()
{
	return m_currentPlayer;
}

/*
	Terminate the thread.
*/
void TTT3DNegamax::stop()
{
	terminate();
	wait();
}

/*
	Protected function.
	For each available move, call Negamax to access the likelihood of a win.
	Negamax algorithm requires every other level's values to be negative (Min's value),
		therefore, calling the negative of negamax will always return the negated value
		and we can just simply search for the maximum value and percolate up the tree.
	Since this tree is big, I'm limiting the depth that it goes down (6 levels).
*/
void TTT3DNegamax::run()
{	/*
		Simple heuristic values.
		-2 = no play; -1 = loss; 0 = draw; 1 = win; otherwise = score of move i.
	*/
	QVector<int> scores(27);

	int cutOff = (27 - m_unoccupiedSq) + 6; // 6 levels depth
	cutOff = cutOff > 27 ? 27 : cutOff;

	int maxScore = -3;
	int maxInd = 0;

	QTime t;
	t.start();

	for (int i = 0; i < 27; i++)
	{
		if (m_boardArray->at(i) == BlankSq)
		{
			makeMove(i, m_computer);	// move is virtual
			scores[i]	= -applyNegamax((27 - m_unoccupiedSq), cutOff);
			undoMove();
		}
		else
			scores[i]	= -2;

		if (scores[i] > maxScore)
		{
			maxScore	= scores[i];
			maxInd		= i;
		}

		if (maxScore > 0)
			break;
	}

	// To simulate the effect of computer thinking.
	while (t.elapsed() < 2000)
	{}

	makeMove(maxInd, m_computer);			// actual "best" move is made.
	emit done(maxInd);				// inform viewboard when it's done.
}

/*
	Negamax function; called from run().
	If it still can't determine a win/loss/draw, continue.
*/
int TTT3DNegamax::applyNegamax(int currDepth, int depthCutOff)
{	/*
		Return values:
		-1 = loss for current player.
		0 = draw for both players.
		1 = win for current player.
	*/
	int state = getResult();

	if ((state == 1) || (state == 2))
	{
		if (m_currentPlayer == state)
			return 1;
		else
			return -1;
	}
	else if ((state == 3) || (currDepth > depthCutOff))	// return 0 if it's a draw or exceeded depth limit.
		return 0;

	QVector<int> scores(27);
	int maxScore = -3;

	for (int i = 0; i < 27; i++)
	{
		if (m_boardArray->at(i) == BlankSq)
		{
			makeMove(i, m_currentPlayer);
			scores[i]	= -applyNegamax(currDepth + 1, depthCutOff);
			undoMove();
		}
		else
			scores[i]	= -2;

		if (scores[i] > maxScore)
			maxScore	= scores[i];

		if (maxScore > 0)
			break;
	}

	return maxScore;
}

/*
	Check board for winning move.
	If all squares are occupied already, then it's a draw.
	For each 8 possible winning moves,
		check to see if all 3 squares have the same value and return that value.
	If there is still no winning move, return 0 (ongoing).
*/
int TTT3DNegamax::getResult()
{	// 0 = ongoing; 1 = max win; 2 = min win; 3 = draw;
	if (m_unoccupiedSq == 0)
		return 3;

	for (int i = 0; i < 49; i++)
	{ // check scoring board
		if ((m_boardArray->at(m_scoring[i][0]) != BlankSq) && (m_boardArray->at(m_scoring[i][0]) == m_boardArray->at(m_scoring[i][1])) && (m_boardArray->at(m_scoring[i][1]) == m_boardArray->at(m_scoring[i][2])))
			return m_boardArray->at(m_scoring[i][0]);
	}

	return 0;
}
