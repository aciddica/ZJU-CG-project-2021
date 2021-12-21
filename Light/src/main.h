#ifndef main_h
#define main_h

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
#include "glm.h"
#include <glut/glut.h>
#include <opengl/gl.h>

using namespace std;

#define GL_SILENCE_DEPRECATION

const int screenWidth = 1200; //800
const int screenHeight = 1000; //800

GLfloat eye[3] = { 0, 0, 8 };
GLfloat center[3] = { 0, 0, 0 };
GLfloat dir[3] = { 0, 0, 1.5f };
GLfloat eyeVar = 1.0f;

int operation = 1; //0:lightPos  1:lightColor
bool ifmainlight = true;
bool ifflashlight = false;
bool ifspotlight = false;

//灯
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
int light_color_index = 0;
GLfloat light_color[][10] = { \
    //Kotori White
    {1.0f, 1.0f ,1.0f, 1.0f},\
    //Hono Orange
    {1.0f, 0.5f, 0.0f, 1.0f},\
    //Umi Blue
    {0.0f, 0.0f, 1.0f, 1.0f},\
    //Maki Red
    {1.0f, 0.0f, 0.0f, 1.0f},\
    //Rin Yellow
    {1.0f, 1.0f, 0.0f, 1.0f},\
    //Hanayo Green
    {0.0f, 1.0f, 0.0f, 1.0f},\
    //Eli Cyan
    {0.0f, 1.0f, 1.0f, 1.0f},\
    //Nozomi Violet
    {0.5f, 0.0f, 0.5f, 1.0f},\
    //Nico Pink
    {1.0f, 0.0f, 0.5f, 1.0f} \
};

//大灯
GLfloat MainLightPos[] = { 0, 5, 10 }, MainLightPosVar = 1.0f;
GLfloat MainLightColor[] = { 1, 1, 1 }, MainLightColorVar = 0.1f;
//GLfloat light_pos[] = {5, 5, 5, 1};

//手电筒
GLfloat FLambient[] = {1, 1, 1, 1};
GLfloat FLposition[] = {eye[0], eye[1], eye[2], 1};
GLfloat FLspotangle = 30.0f;
GLfloat FLlightDir[] = {-dir[0], -dir[1], -dir[2]};


GLfloat fdir[]={0,0,-1,1};

//聚光灯
GLfloat SPambient[] = {1, 1, 1, 1};
GLfloat SPposition[] = {0, 30, 0, 1};
GLfloat SPspotangle = 30.0f;
GLfloat SPlightDir[] = {0, -1, 0, 1};

void key(unsigned char k, int x, int y);
void display();
void initGlobalScene();
void reshape(int w, int h);
void init();
void idle();

#endif /* main_h */
