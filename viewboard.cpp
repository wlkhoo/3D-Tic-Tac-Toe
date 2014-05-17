/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		viewboard.cpp
	CLASS:		ViewBoard
	DETAILS:	Secondary GUI of the program, which handles the game board.
*/
#include "viewboard.h"

/*
	Constructor
	Initialize the board and the negamax thread.
	Setup the board and display the cube.
	Setup a "New Game" button
	Setup two labels that show who is the current player
*/
ViewBoard::ViewBoard(QWidget *p, Qt::WindowFlags f)
	: QWidget (p, f)
{
	m_computerEnabled	= false;

	m_cubeWid 		= new Cube();
	connect(m_cubeWid, SIGNAL(marked(int)), this, SLOT(humanMove(int)));

	QPushButton *button 	= new QPushButton(tr("New Game"));
	button			->setFont(QFont("Times", 30));
	connect(button, SIGNAL(clicked()), this, SLOT(newGame()));

	m_labelMax		= new QLabel;
	m_labelMax		->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_labelMax		->setAlignment(Qt::AlignCenter);
	m_labelMax		->setFont(QFont("Times", 30));
	m_labelMax		->setAutoFillBackground(true);

	m_labelMin		= new QLabel;
	m_labelMin		->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_labelMin		->setAlignment(Qt::AlignCenter);
	m_labelMin		->setFont(QFont("Times", 30));
	m_labelMin		->setAutoFillBackground(true);

	QVBoxLayout *layout1	= new QVBoxLayout;
	layout1			->addWidget(button);
	layout1			->addWidget(m_labelMax);
	layout1			->addWidget(m_labelMin);
	layout1			->setSpacing(200);

	QHBoxLayout *layout2	= new QHBoxLayout;
	layout2			->addWidget(m_cubeWid);
	layout2			->setStretchFactor(m_cubeWid, 2);
	layout2			->addLayout(layout1);
	layout2			->setStretchFactor(layout1, 1);
	layout2			->setSpacing(30);

	setLayout(layout2);

	m_negamax		= new TTT3DNegamax();
	connect(m_negamax, SIGNAL(done(int)),	this, SLOT(computerMove(int)));
	connect(m_negamax, SIGNAL(victory(int)),this, SLOT(winOrDraw(int)));
}

/*
	New game.
	Setting up the players and display the appropriate labels.
	Assigning color: 1st player get Green; 2nd player get Blue.
*/
void ViewBoard::setPlayers(Player one, Player two)
{
	QString text1, text2;

	m_player1 = one;
	m_player2 = two;

	if (one == Human)
	{
		text1 				= tr("Player 1");
		if (two == Human)
		{
			text2 			= tr("Player 2");
			m_computerEnabled	= false;
		}
		else
		{
			text2 			= tr("Computer 2");
			m_computerEnabled	= true;
		}
	}
	else
	{
		text1 				= tr("Computer 1");
		m_computerEnabled		= true;
		if (two == Human)
			text2 			= tr("Player 2");
		else
			text2 			= tr("Computer 2");
	}

	m_labelMax	->setText(tr("%1 : [Green]").arg(text1));
	m_labelMin	->setText(tr("%1 : [Blue]").arg(text2));
	reset();

	if (m_player1 == Computer)
	{	// First player is a computer, lock keyboard inputs and activate negamax.
		m_cubeWid	->releaseKeyboard();
		setCursor(QCursor(Qt::BusyCursor));
		m_negamax	->computerMove(1);
	}
	else
		m_cubeWid	->grabKeyboard();
}

/*
	Human player has selected a move.
	Find out which square is activated; translate the location in grid
		to 1D array of the game board (stored in TTT3DNegamax).
*/
void ViewBoard::humanMove(int move)
{	// move comes from Cube.
	int current;
	bool good;
	current		= m_negamax->currentPlayer();

	// not blank, return
	good = m_cubeWid->markCube((Cube::PlayerCube)(current));
	if (!good)
		return;

	// make the move in TTT3DNegamax (marking the board array).
	m_negamax	->makeMove(move, current);
	if (m_negamax	->checkVictory())
		return;

	changeTurn();
	current		^= 0x3;

	/*
		Find out who is the next one to go.
		Mainly concern if the next player is a computer,
			if not, this function will return and await the next human player to click.
		When computer is in the process of moving, all keyboard inputs are blocked.
	*/
	if (current == 1)
	{
		if (m_player1 == Computer)
		{
			m_cubeWid	->releaseKeyboard();
			setCursor(QCursor(Qt::BusyCursor));
			m_negamax	->computerMove(1);
		}
	}
	else if (current == 2)
	{
		if (m_player2 == Computer)
		{
			m_cubeWid	->releaseKeyboard();
			setCursor(QCursor(Qt::BusyCursor));
			m_negamax	->computerMove(2);
		}
	}
}

/*
	Computer turn to move
	Connected from TTT3DNegamax.
	This function will only be called if the game is active and computer is enabled.
*/
void ViewBoard::computerMove(int move)
{
	if (!m_computerEnabled)
		return;

	// translating 1D location in the array to 3D location for the Cube to display.
	int x, y, z, temp, current;
	z 		= move%3;
	temp 		= move/3;
	y		= temp%3;
	x		= temp/3;

	// set the location.
	m_cubeWid	->setAxis(x, y, z);

	current		= m_negamax->currentPlayer();
	// mark the cube.
	m_cubeWid	->markCube((Cube::PlayerCube)(current ^= 0x3));

	if(m_negamax	->checkVictory())
		return;

	m_cubeWid	->grabKeyboard();
	changeTurn();
	current		^= 0x3;

	setCursor(QCursor(Qt::ArrowCursor));

	/*
		Find out who is the next one to go.
		Mainly concern if the next player is a computer,
			if not, this function will return and await the next human player to click.
		When computer is in the process of moving, all keyboard inputs are blocked.
	*/
	if (current == 1)
	{
		if (m_player1 == Computer)
		{
			m_cubeWid	->releaseKeyboard();
			setCursor(QCursor(Qt::BusyCursor));
			m_negamax	->computerMove(1);
		}
	}
	else if (current == 2)
	{
		if (m_player2 == Computer)
		{
			m_cubeWid	->releaseKeyboard();
			setCursor(QCursor(Qt::BusyCursor));
			m_negamax	->computerMove(2);
		}
	}
}

/*
	This function will only be called (from TTT3DNegamax) if one of the 3 possible outcomes is indicated.
	Display the game result in a dialog box.
*/
void ViewBoard::winOrDraw(int result)
{
	m_negamax		->stop();
	m_cubeWid		->releaseKeyboard();

	setCursor(QCursor(Qt::ArrowCursor));

	QString text;
	switch(result)
	{
		case 1:
			if ((m_player1 == Human) && (m_player1 != m_player2))
				text = tr("You beat the computer! Victory for you, Player 1.");
			else if ((m_player1 == Human) && (m_player1 == m_player2))
				text = tr("Player 1 win! Player 2 suck");
			else if ((m_player1 == Computer) && (m_player1 != m_player2))
				text = tr("You got beaten by a computer. You lose!");
			else if ((m_player1 == Computer) && (m_player1 == m_player2))
				text = tr("Computer 1 beat computer 2!!!");
			break;

		case 2:
			if ((m_player2 == Human) && (m_player1 != m_player2))
				text = tr("You beat the computer! Victory for you, Player 2.");
			else if ((m_player2 == Human) && (m_player1 == m_player2))
				text = tr("Player 2 win! Player 1 suck");
			else if ((m_player2 == Computer) && (m_player1 != m_player2))
				text = tr("You got beaten by a computer. You lose!");
			else if ((m_player2 == Computer) && (m_player1 == m_player2))
				text = tr("Computer 2 beat computer 1!!!");
			break;

		case 3:
			text = tr("Draw! Try again.");
			break;
	}

	QMessageBox::information(this, tr("Game Result"), tr("%1").arg(text));
}

/*
	Reset the board and display.
*/
void ViewBoard::reset()
{
	m_negamax	->reset();
	m_cubeWid	->reset();

	changeTurn();
}

/*
	New game is triggered, ask the player if they want to start a new round.
*/
void ViewBoard::newGame()
{
	int ret = QMessageBox::information(this, tr("3D Tic-Tac-Toe"), tr("Start new round?"), QMessageBox::Yes | QMessageBox::No);

	if (ret == QMessageBox::No)	// Players do not want to start a new round, return to defalut view.
		emit endTurn();		// This signal connects to MainWindow.
	else				// Players want to start a new round with the same configuration.
	{	// Reset
		m_negamax		->stop();
		m_cubeWid		->releaseKeyboard();
		setPlayers(m_player1, m_player2);
	}
}

/*
	Change the labels to display who is the current player by asking TTT3DNegamax.
*/
void ViewBoard::changeTurn()
{
	if (m_negamax->currentPlayer() == 1)
	{
		m_labelMax	->setPalette(QPalette(Qt::yellow));
		m_labelMin	->setPalette(QPalette(Qt::white));
	}
	else
	{
		m_labelMin	->setPalette(QPalette(Qt::yellow));
		m_labelMax	->setPalette(QPalette(Qt::white));
	}
}
