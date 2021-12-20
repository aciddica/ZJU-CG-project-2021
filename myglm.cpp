#include"myglm.h"
void cross(double v1[], double v2[], double res[])
{
	res[0] = v1[1] * v2[2] - v2[1] * v1[2];
	res[1] = v1[2] * v2[0] - v2[2] * v1[0];
	res[2] = v1[0] * v2[1] - v2[0] * v1[1];
	if (res[0] * res[0] + res[1] * res[1] + res[2] * res[2] == 0)//parallel
	{
		res[0] = 1;
		res[1] = 0;
		res[2] = 0;
	}
	normalize(res);
	/*printf("v1 : %lf %lf %lf\n", v1[0], v1[1], v1[2]);
	printf("v2 : %lf %lf %lf\n", v2[0], v2[1], v2[2]);
	printf("res : %lf %lf %lf\n", res[0], res[1], res[2]);*/
}
void normalize(double v[])
{
	double norm = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
}
Point::Point()
{
	x = 0;
	y = 0;
	z = 0;
}
Point::Point(double ix, double iy, double iz)
{
	x = ix;
	y = iy;
	z = iz;
}
MapUnit::MapUnit()
{
	status = 0;
	leftbottom = Point();
	rightup = Point();
}
MapUnit::MapUnit(int s, Point lu, Point rb)
{
	leftbottom = lu;
	rightup = rb;
	status = s;
}
void MapUnit::drawGrid()
{
	//printf("gird:(%lf,%lf) (%lf,%lf)\n", leftbottom.x, leftbottom.y, rightup.x, rightup.y);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(leftbottom.x, leftbottom.y, leftbottom.z);
	glVertex3f(leftbottom.x, rightup.y, leftbottom.z);
	glVertex3f(rightup.x, rightup.y, rightup.z);
	glVertex3f(rightup.x, leftbottom.y, rightup.z);
	glEnd();
}
void MapUnit::drawModel()
{
	//printf("model:(%lf,%lf)  %d \n", (rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2.0, status);
	glPushMatrix();
	glTranslatef((rightup.x+leftbottom.x)/2.0, (rightup.y + leftbottom.y) / 2.0, (rightup.z + leftbottom.z) / 2.0);
	if (status == 1)
	{
		glutSolidCube(GirdLen-0.05);
	}
	glPopMatrix();
}
Map::Map()
{
	for (int i = 0; i < MAPSIZE; i++)
	{
		for (int j = 0; j < MAPSIZE; j++)
		{
			
			Point v1(-0.5 * MAPBOUND + i * GirdLen, -0.5 * MAPBOUND + j * GirdLen, 0);
			Point v2(-0.5 * MAPBOUND + (i + 1) * GirdLen, -0.5 * MAPBOUND + (j + 1) * GirdLen, 0);
			map[i][j] = MapUnit(0, v1, v2);
		}
	}
}
void Map::drawMap()
{
	//printf("new drawing\n");
	for (int i = 0; i < MAPSIZE; i++)
	{
		for (int j = 0; j < MAPSIZE; j++)
		{
			map[i][j].drawGrid();
			map[i][j].drawModel();
		}
	}
}
void Map::Select(double x, double y, int status)
{
	//out of boundary
	if (x <= -0.5 * MAPBOUND && y <= -0.5 * MAPBOUND)
	{
		map[0][0].status = status;
		return;
	}
	else if (x <= -0.5 * MAPBOUND && y >= 0.5 * MAPBOUND)
	{
		map[0][MAPSIZE - 1].status = status;
		return;
	}
	else if (x >= 0.5 * MAPBOUND && y >= 0.5 * MAPBOUND)
	{
		map[MAPSIZE - 1][MAPSIZE - 1].status = status;
		return;
	}
	else if (x >= 0.5 * MAPBOUND && y <= -0.5 * MAPBOUND)
	{
		map[MAPSIZE - 1][0].status = status;
		return;
	}
	int i;
	for (i = 0; i < MAPSIZE; i++)
	{
		double t = -0.5 * MAPBOUND + (i+1) * GirdLen;
		if (x <= t) break;
	}
	int j;
	for (j = 0; j < MAPSIZE; j++)
	{
		double t = -0.5 * MAPBOUND + (j+1) * GirdLen;
		if (y <= t) break;
	}
	//printf("[%d][%d]\n", i, j);
	if (i < MAPSIZE && j < MAPSIZE) map[i][j].status = status;
	return;

}
