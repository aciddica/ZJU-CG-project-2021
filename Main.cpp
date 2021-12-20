#include<stdlib.h>
#include<iostream>
#include"myglm.h"
//#include<gl/glut.h>
#define WindowWidth 720
#define WindowHeight 720
#define RotateChange 0.02
using namespace std;
double CameraPosition[] = { 0,0,10 };
double CameraDirection[] = { 0,0,-1 };
double up[] = { 0,1,0 };
double TargetDistance = 10;
Map BackGroundScene = Map();
bool CameraRotate = false;
void lighttest()
{
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = { 5,5,5,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);
}
void SelectMap(int x, int y)//select a block from the map
{
	int viewport[4];
	double modelview[16];
	double projection[16];
	float winx, winy, winz;
	double objx, objy, objz;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	winx = (float)x;
	winy = (float)viewport[3] - (float)y - 1.0f;
	glReadBuffer(GL_BACK);
	glReadPixels(x, int(winy), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winz);
	gluUnProject((GLdouble)winx, (GLdouble)winy, (GLdouble)winz, modelview, projection, viewport, &objx, &objy, &objz);
	double ClickPosition[3];
	ClickPosition[0] = objx; ClickPosition[1] = objy; ClickPosition[2] = objz;
	printf("camera dir: %lf %lf %lf\n", CameraDirection[0], CameraDirection[1], CameraDirection[2]);
	double N[3] = { -CameraDirection[0],-CameraDirection[1],-CameraDirection[2] };
	double U[3];
	double left[3];
	double CameraUp[3];
	cross(CameraDirection, up, left);
	cross(left, CameraDirection, CameraUp);
	cross(N, CameraUp, U);
	double V[3];
	cross(N, U, V);
	double WorldPos[3];
	printf("cam up: %lf %lf %lf\n", CameraUp[0], CameraUp[1], CameraUp[2]);
	WorldPos[0] = U[0] * ClickPosition[0] + V[0] * ClickPosition[1] + CameraDirection[0] * ClickPosition[2] + CameraPosition[0];
	WorldPos[1] = U[1] * ClickPosition[0] + V[1] * ClickPosition[1] + CameraDirection[1] * ClickPosition[2] + CameraPosition[1];
	WorldPos[2] = U[2] * ClickPosition[0] + V[2] * ClickPosition[1] + CameraDirection[2] * ClickPosition[2] + CameraPosition[2];
	//normalize(WorldPos);
	//printf("pos: %lf %lf %lf\n", CameraPosition[0], CameraPosition[1], CameraPosition[2]);
	//printf("world: %lf %lf %lf\n", WorldPos[0], WorldPos[1], WorldPos[2]);
	
	double ClickDirection[3] = { WorldPos[0] - CameraPosition[0],WorldPos[1] - CameraPosition[1],WorldPos[2] - CameraPosition[2] };
	if (ClickDirection[2] == 0) return; //parallel to xoy
	//normalize(ClickDirection);
	printf("world dir: %lf %lf %lf\n", ClickDirection[0], ClickDirection[1], ClickDirection[2]);
	double len = (-1 * CameraPosition[2]) / ClickDirection[2];
	printf("hit: %lf %lf %lf\n", CameraPosition[0] + len * ClickDirection[0], CameraPosition[1] + len * ClickDirection[1], CameraPosition[2] + len * ClickDirection[2]);
	BackGroundScene.Select(CameraPosition[0] + len * ClickDirection[0], CameraPosition[1] + len * ClickDirection[1], 1);
}
void keyboard(unsigned char k, int x, int y)
{

	double CameraStep = 0.2;
	double CameraRight[3];
	cross(up, CameraDirection, CameraRight);
	//cout <<"right " << CameraRight[0] << " " << CameraRight[1] << " " << CameraRight[2] << endl;
	switch (k)
	{
		case 'w':
		{
			CameraPosition[0] += CameraDirection[0] * CameraStep;
			CameraPosition[1] += CameraDirection[1] * CameraStep;
			CameraPosition[2] += CameraDirection[2] * CameraStep;
		} break;
		case 's':
		{
			CameraPosition[0] -= CameraDirection[0] * CameraStep;
			CameraPosition[1] -= CameraDirection[1] * CameraStep;
			CameraPosition[2] -= CameraDirection[2] * CameraStep;
		} break;
		case 'a':
		{
			CameraPosition[0] += CameraRight[0] * CameraStep;
			CameraPosition[1] += CameraRight[1] * CameraStep;
			CameraPosition[2] += CameraRight[2] * CameraStep;
		} break;
		case 'd':
		{
			CameraPosition[0] -= CameraRight[0] * CameraStep;
			CameraPosition[1] -= CameraRight[1] * CameraStep;
			CameraPosition[2] -= CameraRight[2] * CameraStep;
		} break;
		case ' ':
		{
			CameraPosition[0] += up[0] * CameraStep;
			CameraPosition[1] += up[1] * CameraStep;
			CameraPosition[2] += up[2] * CameraStep;
		} break;
		case 'x':
		{
			CameraPosition[0] -= up[0] * CameraStep;
			CameraPosition[1] -= up[1] * CameraStep;
			CameraPosition[2] -= up[2] * CameraStep;
		}
	}
	//gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraPosition[0] + CameraDirection[0] * TargetDistance, CameraPosition[1] + CameraDirection[1] * TargetDistance, CameraPosition[2] + CameraDirection[2] * TargetDistance, 0.0, 1.0, 0.0);
}
void MouseFunc(int button, int state, int x, int y)      // 鼠标函数，  单击右键允许移动摄像头， 松开右键即不允许移动
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		CameraRotate = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		CameraRotate = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		SelectMap(x, y);
	}
}
void MotionFunc(int x, int y)
{
	y = WindowHeight - y;
	static int LastX = 0;
	static int LastY = 0;
	static float CameraRotateX = 0;
	static float CameraRotateY = 0;
	if (CameraRotate)
	{
		if (x > LastX)
		{
			CameraRotateX += RotateChange;
			LastX = x;
		}
		else
		{
			CameraRotateX -= RotateChange;
			LastX = x;
		}
		if (y > LastY)
		{
			CameraRotateY += RotateChange;
			LastY = y;
		}
		else
		{
			CameraRotateY -= RotateChange;
			LastY = y;
		}
	}
	CameraDirection[0] = sin(CameraRotateX);	
	CameraDirection[1] = sin(CameraRotateY);
	CameraDirection[2] = -cos(CameraRotateX);

	//printf("dir: %lf %lf %lf\n", CameraDirection[0], CameraDirection[1], CameraDirection[2]);
	

	normalize(CameraDirection);
}
void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	gluPerspective(45.0f, whRatio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}
void init()
{
	glEnable(GL_DEPTH_TEST);
	
	//lighttest();
}
void idle()
{
	glutPostRedisplay();
}
void display()
{
	//cout <<"camera: "<< CameraPosition[0] + CameraDirection[0] * TargetDistance << " " << CameraPosition[1] + CameraDirection[1] * TargetDistance << " " << CameraPosition[2] + CameraDirection[2] * TargetDistance << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraPosition[0] + CameraDirection[0] * TargetDistance, CameraPosition[1] + CameraDirection[1] * TargetDistance, CameraPosition[2] + CameraDirection[2] * TargetDistance, 0.0, 1.0, 0.0);
	BackGroundScene.drawMap();
	glutSwapBuffers();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WindowHeight, WindowWidth);
	glutCreateWindow("*******");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
