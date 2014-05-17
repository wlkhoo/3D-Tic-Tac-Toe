/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		ttt3dnegamax.h
	CLASS:		TTT3DNegamax
	DETAILS:	Negamax.
			Inherits from QThread
*/
#ifndef			TTT3DNEGAMAX_H
#define			TTT3DNEGAMAX_H

#include 		<QMutex>
#include 		<QThread>
#include 		<QWaitCondition>
#include		<QVector>
#include		<QStack>
#include		<QTime>

/* board representation
	(bottom)	(mid)		(top)
	level 1		level 2		level 3

	0   9  18	3  12  21	6  15  24
	1  10  19	4  13  22	7  16  25
	2  11  20	5  14  23	8  17  26

	Below is a list of winning squares; there are 49 possible wins.
*/

const int m_scoring[49][3] = {
	// row & col
	{0, 1, 2}, 	{3, 4, 5}, 	{6, 7, 8},
	{9, 10, 11}, 	{12, 13, 14}, 	{15, 16, 17},
	{18, 19, 20}, 	{21, 22, 23}, 	{24, 25, 26},

	{0, 9, 18},	{1, 10, 19},	{2, 11, 20},
	{3, 12, 21},	{4, 13, 22},	{5, 14, 23},
	{6, 15, 24},	{7, 16, 25},	{8, 17, 26},

	{0, 3, 6},	{1, 4, 7},	{2, 5, 8},
	{9, 12, 15},	{10, 13, 16},	{11, 14, 17},
	{18, 21, 24},	{19, 22, 25},	{20, 23, 26},

	// diag
	{0, 10, 20}, 	{18, 10, 2},	{3, 13, 23},
	{21, 13, 5}, 	{6, 16, 26},	{24, 16, 8},

	{0, 12, 24},	{6, 12, 18},	{6, 4, 2},	{0, 4, 8},
	{2, 14, 26},	{8, 14, 20},	{20, 22, 24},	{18, 22, 26},
	{7, 13, 19},	{1, 13, 25},	{9, 13, 17},	{11, 13, 15},

	{0, 13, 26},	{6, 13, 20},	{2, 13, 24},	{8, 13, 18}
};

class TTT3DNegamax : public QThread
{
			Q_OBJECT

public:
			TTT3DNegamax	(QObject *p = 0);
			~TTT3DNegamax	();
	enum		SqCube		{BlankSq, MaxSq, MinSq};
	void		reset		();
	void		makeMove	(int, int);
	void		undoMove	();
	void		computerMove	(int);
	bool		checkVictory	();
	int		currentPlayer	();
	void		stop		();

signals:
	void		done		(int);
	void		victory		(int);

protected:
	void		run		();

private:
	int		applyNegamax	(int, int);
	int		getResult	();

	QMutex		mutex;
	QWaitCondition	cond;

	QVector<int>	*m_boardArray;
	QStack<int>	*m_undoStack;

	int		m_unoccupiedSq;
	int		m_currentPlayer;
	int		m_computer;
};
#endif
