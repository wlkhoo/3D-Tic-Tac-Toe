/*
	Copyright (C) 2008 by Wai Khoo

	Entry point of 3D Tic Tac Toe.
	Written in Qt 4.4.0.
	Multi-threaded.
*/
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
        QApplication a (argc, argv);

        MainWindow w;
        w.showMaximized ();

        return a.exec();
}
