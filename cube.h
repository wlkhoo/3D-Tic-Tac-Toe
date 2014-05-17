/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		cube.h
	CLASS:		Cube
	DETAILS:	Visualize the 3D Tic Tac Toe cube.
			Inherits from QGLWidget, which is OpenGL
*/
#ifndef			CUBE_H
#define			CUBE_H

#include		<QtOpenGL>

class Cube : public QGLWidget
{
			Q_OBJECT

public:
			Cube		(QWidget *p = 0, QGLWidget *shareWidget = 0);
			~Cube		();
	enum		PlayerCube	{Blank, MaxCube, MinCube};
	void		changeXAxis	(int);
	void		changeYAxis	(int);
	void		changeZAxis	(int);
	void		setAxis		(int, int, int);
	bool		markCube	(PlayerCube);
	void		reset		();

signals:
	void		marked		(int);

protected:
	void		initializeGL	();
	void		paintGL		();
	void		resizeGL	(int, int);
	void 		mousePressEvent	(QMouseEvent*);
	void		mouseMoveEvent	(QMouseEvent*);
	void		keyPressEvent	(QKeyEvent*);

private:
	GLuint		makeCube	(bool, bool, PlayerCube);
	void		drawCube	(GLuint, GLdouble, GLdouble, GLdouble);
	void		updateCube	();

	void 		normalizeAngle	(int*);
	void 		setXRotation	(int);
	void 		setYRotation	(int);
     	void 		setZRotation	(int);

	GLuint 		cubeObj[3][3][3];

	PlayerCube	markedField[3][3][3];

	int		currX;
	int		currY;
	int		currZ;

	int 		xRot;
	int 		yRot;
	int 		zRot;
	QPoint 		lastPos;

	QColor		m_maxCube;
	QColor		m_minCube;
	QColor 		m_blankCube;
};
#endif
