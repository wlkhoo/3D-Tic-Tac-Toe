/*
	Copyright (C) 2008 by Wai Khoo

	AUTHOR:		Wai Khoo
	FILE: 		cube.cpp
	CLASS:		Cube
	DETAILS:	Visualize the 3D Tic Tac Toe cube.
			Inherits from QGLWidget, which is OpenGL
*/
#include	"cube.h"

/*
	Constructor
	Setting up the cube, default rotation angle, and default cube selected.
	Assigning color: 1st player is green; 2nd player is blue; white for blank.
*/
Cube::Cube(QWidget *p, QGLWidget *shareWidget)
	: QGLWidget(p, shareWidget)
{
	xRot 		= 300;
	yRot 		= 360;
	zRot 		= 0;

	currX 		= 0;
	currY		= 2;
	currZ		= 0;

	m_maxCube	= QColor(Qt::green);
	m_minCube	= QColor(Qt::blue);
	m_blankCube	= QColor(Qt::white);

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				cubeObj[x][y][z] = 0;
				markedField[x][y][z] = Blank;
			}
}

/*
	Destructor
	delete opengl list which contains cube specifications.
*/
Cube::~Cube()
{
	makeCurrent();
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
				glDeleteLists(cubeObj[x][y][z], 1);
}

/*
	Set/selection function; called within this class.
	set the x axis to dx and upate the cube.
*/
void Cube::changeXAxis(int dx)
{
	currX	+= dx;

	currX	= currX > 2 ? 2 : currX;
	currX	= currX < 0 ? 0 : currX;

	updateCube();
}

/*
	Set/selection function; called within this class.
	set the y axis to dy and update the cube.
*/
void Cube::changeYAxis(int dy)
{
	currY	+= dy;

	currY	= currY > 2 ? 2 : currY;
	currY	= currY < 0 ? 0 : currY;

	updateCube();
}

/*
	Set/selection function; called within this class.
	set the z axis to dz and update the cube
*/
void Cube::changeZAxis(int dz)
{
	currZ	+= dz;

	currZ	= currZ > 2 ? 2 : currZ;
	currZ	= currZ < 0 ? 0 : currZ;

	updateCube();
}

/*
	Set/selection function; allow other class to modify the cube selection
	set the axis to x, y, z and update the cube.
*/
void Cube::setAxis(int x, int y, int z)
{
	currX	= x;
	currY	= y;
	currZ	= z;

	updateCube();
}

/*
	Only allow to mark blank square.
*/
bool Cube::markCube(PlayerCube p)
{
	if (markedField[currX][currY][currZ] != Blank)
		return false;

	markedField[currX][currY][currZ] = p;
	updateCube();
	return true;
}

/*
	reset the cube to default configurations/values.
*/
void Cube::reset()
{
	xRot 		= 300;
	yRot 		= 360;
	zRot 		= 0;

	currX 		= 0;
	currY		= 2;
	currZ		= 0;

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				cubeObj[x][y][z] = 0;
				markedField[x][y][z] = Blank;
			}
	updateCube();
}

/*
	Protected function.
	initialize opengl.
*/
void Cube::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
     	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);

	updateCube();
}

/*
	Protected function
	this function is called every times a cube is updated.
*/
void Cube::paintGL()
{
	QColor m_color(Qt::lightGray);

	qglClearColor(m_color.lighter(105));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslated(0.0, 0.0, -12.0);
	glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

	int indX, indY, indZ;
	double x, y, z;
	for (indX = 0, x = -1.0; x <= 1.0; x++, indX++)
		for (indY = 0, y = -1.0; y <= 1.0; y++, indY++)
			for (indZ = 0, z = -1.0; z <= 1.0; z++, indZ++)
				drawCube(cubeObj[indX][indY][indZ], x, y, z);
}

/*
	Protected function
	this function is called whenever the window is resize.
*/
void Cube::resizeGL(int w, int h)
{
	int side = qMin(w, h);
	glViewport((w - side)/2, (h - side)/2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
     	glTranslated(0.0, 0.0, -40.0);
}

/*
	A cube is made based on 3 parameters (selected, active, who owns it)
	If a cube is selected, it will paint a red border.
	If a cube is active, it will paint white with no transparency.
	If a cube is owned, it will paint the appropriate color.
	This function return a "specification" for the cube, which is stored in a 3D array.
*/
GLuint Cube::makeCube(bool selectCube, bool transparent, PlayerCube p)
{
	QColor m_lineColor, m_cubeColor;
	static const int coords[6][4][3] = {
	         { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
	         { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
	         { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
	         { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
	         { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
	         { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
     	};

	int alpha	= transparent? 100 : 255;

    	m_maxCube.setAlpha(alpha);
    	m_minCube.setAlpha(alpha);
    	m_blankCube.setAlpha(alpha);

    	switch (p)
    	{
		case Blank:
			m_cubeColor = m_blankCube;
			break;
		case MaxCube:
			m_cubeColor = m_maxCube;
			break;
		case MinCube:
			m_cubeColor = m_minCube;
			break;
	}

     	GLuint list	= glGenLists(1);
     	glNewList(list, GL_COMPILE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (selectCube)
	{
		glLineWidth(2.0);
		m_lineColor = Qt::red;
	}
	else
	{
		glLineWidth(1.0);
		m_lineColor = Qt::black;
	}

     	for (int i = 0; i < 6; i++)
     	{
		qglColor(m_lineColor);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2d(j == 0 || j == 3, j == 0 || j == 1);
			glVertex3d(0.2 * coords[i][j][0], 0.2 * coords[i][j][1], 0.2 * coords[i][j][2]);
		}
		glEnd();

		qglColor(m_cubeColor);
		glBegin(GL_QUADS);
		glEdgeFlag(false);
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2d(j == 0 || j == 3, j == 0 || j == 1);
			glVertex3d(0.195 * coords[i][j][0], 0.195 * coords[i][j][1], 0.195 * coords[i][j][2]);
		}
		glEnd();
	}
	glEndList();
	return list;
}

/*
	Draw all the cubes based on the "specification" list.
*/
void Cube::drawCube(GLuint cube, GLdouble dx, GLdouble dy, GLdouble dz)
{
	glPushMatrix();
	glTranslated(dx, dy, dz);
	glScaled(1.4, 1.4, 1.4);
	glCallList(cube);
	glPopMatrix();
}

/*
	Every times a cube has changed,
		it will generate a new cube with new parameter to reflect the current state of the game.
*/
void Cube::updateCube()
{
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				if(z == currZ)
				{
					if (x == currX && y == currY)
						cubeObj[x][y][z] = makeCube(true, false, markedField[x][y][z]);
					else
						cubeObj[x][y][z] = makeCube(false, false, markedField[x][y][z]);
				}
				else
					cubeObj[x][y][z] = makeCube(false, true, markedField[x][y][z]);
			}
	updateGL();
}

/*
	Set function; called within this class.
	set the x angle of the cube.
*/
void Cube::setXRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != xRot)
	{
		xRot = angle;
		updateGL();
	}
}

/*
	Set function; called within this class.
	set the y angle of the cube.
*/
void Cube::setYRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != yRot)
	{
		yRot = angle;
		updateGL();
	}
}

/*
	Set function; called within this class.
	set the z angle of the cube.
*/
void Cube::setZRotation(int angle)
{
	normalizeAngle(&angle);
	if (angle != zRot)
	{
		zRot = angle;
		updateGL();
	}
}

/*
	normalize the angle.
	setting the rotation speed to 1/16 turn per pixel
*/
void Cube::normalizeAngle(int *angle)
{
	while (*angle < 0)
		*angle += 360 * 16;
	while (*angle > 360 * 16)
		*angle -= 360 * 16;
}

/*
	Protected function
	record mouse clicked position; needed for rotation.
*/
void Cube::mousePressEvent(QMouseEvent *e)
{
	lastPos = e->pos();
}

/*
	Protected function
	calculate the mouse displacement and rotate the cube accordingly.
	8 degree/pixel.
*/
void Cube::mouseMoveEvent(QMouseEvent *e)
{
	int dx = e->x() - lastPos.x();
	int dy = e->y() - lastPos.y();

	if (e->buttons() & Qt::LeftButton)
	{
		setXRotation(xRot + 8 * dy);
		setYRotation(yRot + 8 * dx);
	}
	else if (e->buttons() & Qt::RightButton)
	{
		setXRotation(xRot + 8 * dy);
		setZRotation(zRot - 8 * dx);
	}
	lastPos = e->pos();
}

/*
	Protected function
	using keyboard arrow keys and Pg Up/Pg Dn to select cube.
	spacebar is to mark the cube with your color.
*/
void Cube::keyPressEvent(QKeyEvent *e)
{
	// X axis
	if (e->key() == Qt::Key_Left)
		changeXAxis(-1);
	if (e->key() == Qt::Key_Right)
		changeXAxis(1);

	// Y axis
	if (e->key() == Qt::Key_Up)
		changeYAxis(1);
	if (e->key() == Qt::Key_Down)
		changeYAxis(-1);

	// Z axis
	if (e->key() == Qt::Key_PageUp)
		changeZAxis(-1);
	if (e->key() == Qt::Key_PageDown)
		changeZAxis(1);

	// Marking a field
	if (e->key() == Qt::Key_Space)
		emit marked(currX*9 + currY*3 + currZ);
}
