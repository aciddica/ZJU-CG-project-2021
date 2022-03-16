#pragma once
#ifndef main_h
#define main_h

//#define GLFW_EXPOSE_NATIVE_GLX
#include "GL/glew.h"
//GLFW
#include <GLFW\glfw3.h>
#include<windows.h>
#include<GLU.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <memory.h>
#include "nglm.h"
#include"ReadObj.h"
#include"myglm.h"

using namespace std;


#define GL_SILENCE_DEPRECATION
#define WindowWidth 720
#define WindowHeight 720
#define RotateChange (!CameraRotate ? 0.05 : 0.01)
#define HeightLimit 10000
#define RoamLimit 50000
#define BottomLimit -0.5
#define PipelineComponetHeight 250
#define InitialCameraPos 5000
#define InitialRoamCameraPos 1000
using namespace std;
double CameraPosition[] = { 0,0,InitialCameraPos };
double CameraDirection[] = { 0,0,-1 };
double up[] = { 0,1,0 };

double TargetDistance = 10;
Map BackGroundScene = Map();
PipelineMap PipeLine = PipelineMap();
ProductMap product = ProductMap();
bool CameraRotate = false;

/*
	��ʾ�б�
*/
int view_list[10];
int top = 0;
/*end*/
/*switch camera*/
bool CameraSwitch = false;

static int oldmy = -1;
static int oldmx = -1;
int showProduct = 0;

/*added by wys*/
#define PI       3.14159265359

GLdouble    fovy = 60.0;
GLdouble    nearPlane = 10.0;
GLdouble    farPlane = 100.0;

GLfloat     angle = 0.0;
GLfloat     torusAngle = 0.0;

GLfloat     lightPos[] = { 25.0, 25.0, 25.0, 1.0 };
GLfloat     lookat[] = { 0.0, 0.0, 0.0 };
GLfloat     Up[] = { 0.0, 0.0, 1.0 };

GLfloat fdir[] = { 0,0,-1,1 };
GLboolean showShadow = GL_FALSE;
GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat shininess[] = { 0.0 };
GLfloat position[] = { -1.0, 1.0, -1.0, 0.0 };
GLfloat position2[] = { 0.0, 0.0, 2.0, 0.0 };
int index = 0;
GLfloat color[][4] = {
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 0.0f, 1.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f, 1.0f },
};

//截图
#define BMP_Header_Length 54
int pic_num = 0;
#define GL_BGR 0x80E0

void key(unsigned char k, int x, int y);
void display();
void initGlobalScene();
void reshape(int w, int h);
void init();
void idle();

#endif /* main_h */
