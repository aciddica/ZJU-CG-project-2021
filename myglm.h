#include <math.h>
#include<gl/freeglut.h>
#include<stdio.h>
#define MAPSIZE 10
#define MAPBOUND 5.0
extern double CameraPosition[];
extern double CameraDirection[];
extern double TargetDistance;

void cross(double v1[], double v2[], double res[]);
void normalize(double v[]);
//double* CameraMove(int mode);
class Point //the class point
{
public:
	double x, y, z;
	Point();
	Point(double ix, double iy, double iz);
};
class MapUnit //the class of grid in the map
{
public:
	double GirdLen = MAPBOUND / MAPSIZE;
	Point leftbottom;
	Point rightup;
	int status;
	MapUnit();
	MapUnit(int s, Point lu, Point rb);
	void drawGrid();
	void drawModel();
};
class Map
{
public:
	double GirdLen = MAPBOUND / MAPSIZE;
	MapUnit map[MAPSIZE][MAPSIZE];
	Map();
	void drawMap();
	void Select(double x, double y, int status);	//mapping the point to the map surface(z = 0)
};
