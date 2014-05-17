/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		mainwindow.cpp
	CLASS:		MainWindow
	DETAILS:	The main GUI of the program; implementation of MainWindow
*/
#include "mainwindow.h"

/*
	Constructor
	Initialize all actions and menu and set up layout
*/
MainWindow::MainWindow()
{
	createActions();
	createMenus();

	m_widMain 		= new QStackedWidget;
	setCentralWidget(m_widMain);

	m_widDefault 		= new QWidget;
	m_widMain		->addWidget(m_widDefault);

	QLabel *label1 		= new QLabel(tr("Pick a game type in the menu"));
	label1			->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	label1			->setAlignment(Qt::AlignCenter);
	label1			->setFont(QFont("Times", 30));

	QVBoxLayout *layout1 	= new QVBoxLayout;
	layout1			->addWidget(label1);

	m_widDefault		->setLayout(layout1);

	m_viewBoard		= new ViewBoard();
	connect(m_viewBoard, SIGNAL(endTurn()), this, SLOT(endGame()));
	m_widMain		->addWidget(m_viewBoard);

	setWindowTitle(tr("3D Tic-Tac-Toe"));
}

/*
	Menu action
	Human goes first.
*/
void MainWindow::Hfirst()
{
	m_widMain	->setCurrentWidget(m_viewBoard);
	m_viewBoard	->setPlayers(ViewBoard::Human, ViewBoard::Computer);
}

/*
	Menu action
	Computer goes first.
*/
void MainWindow::Cfirst()
{
	m_widMain	->setCurrentWidget(m_viewBoard);
	m_viewBoard	->setPlayers(ViewBoard::Computer, ViewBoard::Human);
}

/*
	Menu action
	Human vs. human.
*/
void MainWindow::HvsH()
{
	m_widMain	->setCurrentWidget(m_viewBoard);
	m_viewBoard	->setPlayers(ViewBoard::Human, ViewBoard::Human);
}

/*
	Menu action
	Computer vs. computer.
*/
void MainWindow::CvsC()
{
	m_widMain	->setCurrentWidget(m_viewBoard);
	m_viewBoard	->setPlayers(ViewBoard::Computer, ViewBoard::Computer);
}

/*
	Menu action
	Terminate current game, go back to default view.
*/
void MainWindow::endGame()
{
	m_widMain	->setCurrentWidget(m_widDefault);
}

/*
	Menu actions
	Initialization.
*/
void MainWindow::createActions()
{
	m_Hfirst 	= new QAction(tr("Human plays first"), this);
	m_Cfirst	= new QAction(tr("Computer plays first"), this);
	m_HvsH		= new QAction(tr("Human vs. Human"), this);
	m_CvsC		= new QAction(tr("Computer vs. Computer"), this);
	m_quit		= new QAction(tr("E&xit"), this);
	m_quit		->setShortcut(tr("Ctrl+Q"));

	connect(m_Hfirst,	SIGNAL(triggered()), this, SLOT(Hfirst()));
	connect(m_Cfirst,	SIGNAL(triggered()), this, SLOT(Cfirst()));
	connect(m_HvsH,		SIGNAL(triggered()), this, SLOT(HvsH()));
	connect(m_CvsC,		SIGNAL(triggered()), this, SLOT(CvsC()));
	connect(m_quit,		SIGNAL(triggered()), qApp, SLOT(quit()));
}

/*
	Menu
	Initialization.
*/
void MainWindow::createMenus()
{
	m_menuHvsC	= new QMenu(tr("Human vs. Computer"), this);
	m_menuHvsC	->addAction(m_Hfirst);
	m_menuHvsC	->addAction(m_Cfirst);

	m_menuGame	= new QMenu(tr("&Game"), this);
	m_menuGame	->addMenu(m_menuHvsC);
	m_menuGame	->addAction(m_HvsH);
	m_menuGame	->addAction(m_CvsC);
	m_menuGame	->addSeparator();
	m_menuGame	->addAction(m_quit);

	menuBar()	->addMenu(m_menuGame);
}
