/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		viewboard.h
	CLASS:		ViewBoard
	DETAILS:	Secondary GUI of the program, which handles the game board.
*/
#ifndef         	VIEWBOARD_H
#define         	VIEWBOARD_H

#include		"cube.h"
#include		"ttt3dnegamax.h"

class ViewBoard : public QWidget
{
			Q_OBJECT

public:
			ViewBoard	(QWidget *p = 0, Qt::WindowFlags f = 0);
	enum		Player		{Human=1, Computer};
	void		setPlayers	(Player, Player);

signals:
	void		endTurn		();

private slots:
	void		humanMove	(int);
	void		computerMove	(int);
	void		winOrDraw	(int);
	void		reset		();
	void		newGame		();

private:
	void		changeTurn	();

	Cube		*m_cubeWid;

	QLabel		*m_labelMax;
	QLabel		*m_labelMin;

	Player		m_player1;
	Player		m_player2;

	bool		m_computerEnabled;

	TTT3DNegamax	*m_negamax;
};
#endif
