/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		mainwindow.h
	CLASS:		MainWindow
	DETAILS:	The main GUI of the program.
*/
#ifndef         	MAINWINDOW_H
#define         	MAINWINDOW_H

#include		"viewboard.h"

class MainWindow : public QMainWindow
{
			Q_OBJECT

public:
			MainWindow();

private slots:
	void		Hfirst();
	void		Cfirst();
	void		HvsH();
	void		CvsC();
	void		endGame();

private:
	void		createActions();
	void		createMenus();

	QMenu		*m_menuGame;
	QMenu		*m_menuHvsC;

	QAction		*m_Hfirst;
	QAction		*m_Cfirst;
	QAction		*m_HvsH;
	QAction		*m_CvsC;
	QAction		*m_quit;

	QStackedWidget 	*m_widMain;
	QWidget		*m_widDefault;

	ViewBoard	*m_viewBoard;
};
#endif
