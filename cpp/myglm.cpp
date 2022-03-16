#include"myglm.h"
/*  Add by KQ
	@ Math operations
	cross: 三维向量叉乘，v1×V2 -> res
	normalize: 单位化，v -> v
*/
void Stage()
{
	glPushMatrix();
	//glColor3f(0.0, 1.0, 1.0);
	glCallList(view_list[1]);
	glPopMatrix();
}
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
/*  Add by KQ
	@ Point-->MapUnit-->Map
*/
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
	//glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);

	//printf("::: %lf %lf\n",leftbottom.x, rightup.y);
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
	glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2.0, (rightup.z + leftbottom.z) / 2.0);
	if (status == 1)
	{
		//glutSolidCube(GirdLen - 0.05);
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
			//map[i][j].drawModel();
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
		double t = -0.5 * MAPBOUND + (i + 1) * GirdLen;
		if (x <= t) break;
	}
	int j;
	for (j = 0; j < MAPSIZE; j++)
	{
		double t = -0.5 * MAPBOUND + (j + 1) * GirdLen;
		if (y <= t) break;
	}
	//printf("[%d][%d]\n", i, j);
	if (i < MAPSIZE && j < MAPSIZE) map[i][j].status = status;
	return;

}

/* Add by KQ
   @ PipelineUnit-->Pipeline

*/
PipelineUnit::PipelineUnit()
{
	type = 0;
	leftbottom = Point();
	rightup = Point();
}

PipelineUnit::PipelineUnit(int s, Point lu, Point rb)
{
	leftbottom = lu;
	rightup = rb;
	type = s;
}

void PipelineUnit::ChangeType(int s)
{
	type = s;
}

void PipelineUnit::drawModel()////根据不同的type绘制不同的流水线组成部分
{

	//printf("draw\n");
	//if (type > 0)
	//{
	//	glPushMatrix();
	//	glColor3f(1.0, 1.0, 1.0);
	//	
	//	glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y)/2, (rightup.z + leftbottom.z) / 2.0);
	//	glScalef(0.5, 1, 1);
	//	glutSolidCube(Len - 0.05);
	//	//glRotatef(-90, 0, 1, 0);
	//	//glutSolidCone(Len / 2, Len * 0.8, slices, stacks);//绘制实心圆锥体,方便指出起点方向

	//	//可以改为下面的画圆球或立方体，此使要选取前面注释掉的第一句glTranslatef函数，注释掉第二句
	//	//glutSolidCube(Len - 0.05);
	//	//glutSolidSphere(Len/2-0.05, slices, stacks);

	//	glPopMatrix();
	//	return;
	//}

	if (isMachine == 1)
	{
		if (modetype == 16)//绘制流水线上的处理装置
		{
			//处理装制用不同颜色的正方体来表示
			glPushMatrix();
			//glColor3f(0.0, 0.0, 1.0);
			glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
			/*glutSolidCube(Len - 0.05);*/
			glRotatef(90, 0, 0, 1);
			glCallList(view_list[0]);
			glCallList(view_list[2]);

			glPopMatrix();
		}

		if (modetype == 24)//绘制流水线上的处理装置
		{
			//处理装制用不同颜色的正方体来表示
			glPushMatrix();
			//glColor3f(0.0, 0.0, 1.0);
			glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
			/*glutSolidCube(Len - 0.05);*/
		
			glCallList(view_list[0]);
			glCallList(view_list[2]);
			glPopMatrix();

		}

		if (modetype == 17)//绘制流水线上的处理装置
		{
			//处理装制用不同颜色的正方体来表示
			glPushMatrix();
			//glColor3f(0.0, 0.0, 1.0);
			glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
			/*glutSolidCube(Len - 0.05);*/
			glRotatef(90, 0, 0, 1);
			glCallList(view_list[0]);
			glCallList(view_list[3]);

			glPopMatrix();
		}

		if (modetype == 25)//绘制流水线上的处理装置
		{
			//处理装制用不同颜色的正方体来表示
			glPushMatrix();
			//glColor3f(0.0, 0.0, 1.0);
			glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
			/*glutSolidCube(Len - 0.05);*/

			glCallList(view_list[0]);
			glCallList(view_list[3]);
			glPopMatrix();

		}



	}

	if (type == 1)//绘制朝向上方的传送带起点
	{

		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 2)//绘制朝向left的传送带起点
	{

		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 3)//绘制朝向down的传送带起点
	{

		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 4)//绘制朝向right的传送带起点
	{

		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 5)//绘制朝向up的流水线终点
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();

	}
	else if (type == 6)//绘制朝向left的传送带终点
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glPopMatrix();

	}
	else if (type == 7)//绘制朝向down的传送带终点
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 8)//绘制朝向right的传送带终点
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 9||type == 17)//绘制普通流水线轨道up-down方向
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 10 || type == 18)//普通流水线轨道left-right方向
	{
		glPushMatrix();
		//glColor3f(0.0, 1.0, 1.0);
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 11 || type == 19)//绘制普通流水线轨道left-up方向
	{

		glPushMatrix();
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}
	else if (type == 12 || type == 20)//绘制普通流水线轨道left-down方向
	{
		glPushMatrix();
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}


	else if (type == 13 ||type == 21)//绘制普通流水线轨道right-up方向
	{
		glPushMatrix();
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}

	else if (type == 14 || type == 22)//绘制普通流水线right-down方向
	{
		glPushMatrix();
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}


	else if (type == 15)//绘制初始的起点
	{
		glPushMatrix();
		glTranslatef((rightup.x + leftbottom.x) / 2.0, (rightup.y + leftbottom.y) / 2, (rightup.z + leftbottom.z) / 2.0);
		glCallList(view_list[0]);
		glRotatef(90, 0, 0, 1);
		glCallList(view_list[0]);
		glPopMatrix();
	}


}





PipelineMap::PipelineMap()
{
	haveBegin = 0;//初始化为没有开始绘制
	BeginPoint.resize(2, -1);

	isEnd = 0;//初始化为没有终点
	nowEndPoint.resize(2, -1);

	for (int i = 0; i < PIPESIZE; i++)
	{
		for (int j = 0; j < PIPESIZE; j++)
		{
			Point v1(-0.5 * MAPBOUND + i * Len, -0.5 * MAPBOUND + j * Len, 0);
			Point v2(-0.5 * MAPBOUND + (i + 1) * Len, -0.5 * MAPBOUND + (j + 1) * Len, 0);
			map[i][j] = PipelineUnit(0, v1, v2);
		}
	}
}



void PipelineMap::drawMap()
{
	for (int i = 0; i < PIPESIZE; i++)
	{
		for (int j = 0; j < PIPESIZE; j++)
		{
			map[i][j].drawModel();
		}
	}
}

void PipelineMap::Select(double x, double y)
{
	//不处理越界情况，只处理在范围内的点击
	int i;
	for (i = 0; i < PIPESIZE; i++)
	{
		double t = -0.5 * MAPBOUND + (i + 1) * Len;
		if (x <= t) break;
	}
	int j;
	for (j = 0; j < PIPESIZE; j++)
	{
		double t = -0.5 * MAPBOUND + (j + 1) * Len;
		if (y <= t) break;
	}


	if (paintMode == 1)//仅画普通的流水线，以及流水线开头和结尾组件
	{

		//printf("[%d][%d]\n", i, j); map[i][j].type = 1;
		if (i < PIPESIZE && j < PIPESIZE)
		{

			if (isEnd == 1)
			{
				return;
			}

			else if (haveBegin == 0)//将某个点修改为起点
			{
				map[i][j].type = 15;
				haveBegin = 1;
				BeginPoint[0] = i;
				BeginPoint[1] = j;
				nowEndPoint[0] = i;
				nowEndPoint[1] = j;
			}
			else if (haveBegin == 1)//将新的点和之前临时的终点相连接
			{
				int lastx = nowEndPoint[0], lasty = nowEndPoint[1];

				if (lastx == i && lasty == j)
				{
					return;
				}
				else if (lastx == i)
				{
					if (lasty < j)
					{
						for (int k = std::min(lasty, j) + 1; k <= std::max(lasty, j); k++)
						{
							// 判断是否寻路成功，若直接到达的路中间被别的点占据，则不进行修改，直接返回
							if (map[i][k].type != 0)
							{
								return;
							}
						}
						for (int k = std::min(lasty, j) + 1; k < std::max(lasty, j); k++)
						{
							map[i][k].type = 17;
						}
						map[i][j].type = 7;

						//调整上一个终点的形状
						if (map[lastx][lasty].type == 6)map[lastx][lasty].type = 11;
						else if (map[lastx][lasty].type == 7)map[lastx][lasty].type = 17;
						else if (map[lastx][lasty].type == 8)map[lastx][lasty].type = 13;
						else if (map[lastx][lasty].type == 15)map[lastx][lasty].type = 1;

						nowEndPoint[0] = i;
						nowEndPoint[1] = j;

					}

					if (lasty > j)
					{
						for (int k = std::min(lasty, j); k < std::max(lasty, j); k++)
						{
							// 判断是否寻路成功，若直接到达的路中间被别的点占据，则不进行修改，直接返回
							if (map[i][k].type != 0)
							{
								return;
							}
						}
						for (int k = std::min(lasty, j) + 1; k < std::max(lasty, j); k++)
						{
							map[i][k].type = 9;
						}
						map[i][j].type = 5;
						//调整上一个终点的形状
						if (map[lastx][lasty].type == 6)map[lastx][lasty].type = 12;
						else if (map[lastx][lasty].type == 5)map[lastx][lasty].type = 9;
						else if (map[lastx][lasty].type == 8)map[lastx][lasty].type = 14;
						else if (map[lastx][lasty].type == 15)map[lastx][lasty].type = 3;
						nowEndPoint[0] = i;
						nowEndPoint[1] = j;

					}

				}


				else if (lasty == j)
				{
					if (lastx < i)
					{
						for (int k = std::min(lastx, i) + 1; k <= std::max(lastx, i); k++)
						{
							// 判断是否寻路成功，若直接到达的路中间被别的点占据，则不进行修改，直接返回
							if (map[k][j].type != 0)
							{
								return;
							}
						}
						for (int k = std::min(lastx, i) + 1; k < std::max(lastx, i); k++)
						{
							map[k][j].type = 10;
						}
						map[i][j].type = 6;

						//调整上一个终点的形状
						if (map[lastx][lasty].type == 6)map[lastx][lasty].type = 10;
						else if (map[lastx][lasty].type == 5)map[lastx][lasty].type = 21;
						else if (map[lastx][lasty].type == 7)map[lastx][lasty].type = 22;
						else if (map[lastx][lasty].type == 15)map[lastx][lasty].type = 4;

						nowEndPoint[0] = i;
						nowEndPoint[1] = j;
					}
					if (lastx > i)
					{
						for (int k = std::min(lastx, i); k < std::max(lastx, i); k++)
						{
							// 判断是否寻路成功，若直接到达的路中间被别的点占据，则不进行修改，直接返回
							if (map[k][j].type != 0)
							{
								return;
							}
						}
						for (int k = std::min(lastx, i) + 1; k < std::max(lastx, i); k++)
						{
							map[k][j].type = 18;
						}
						map[i][j].type = 8;

						//调整上一个终点的形状
						if (map[lastx][lasty].type == 8)map[lastx][lasty].type = 18;
						else if (map[lastx][lasty].type == 5)map[lastx][lasty].type = 19;
						else if (map[lastx][lasty].type == 7)map[lastx][lasty].type = 20;
						else if (map[lastx][lasty].type == 15)map[lastx][lasty].type = 2;

						nowEndPoint[0] = i;
						nowEndPoint[1] = j;
					}
				}

			}
		}





	}
	else if (paintMode == 2)//将流水线上已经存在的普通流水线轨道改为某种加工器件
	{
		if (map[i][j].type != 9 && map[i][j].type != 10 && map[i][j].type != 17 && map[i][j].type != 18)return;//只能在up-down向或left-right向的普通流水线轨道上摆。
		else
		{
			if (map[i][j].isMachine == 0)
			{
				map[i][j].isMachine = 1;
				if (map[i][j].type == 9 || map[i][j].type == 17)
					map[i][j].modetype = 16;
				if (map[i][j].type == 10 || map[i][j].type == 18)
					map[i][j].modetype = 24;
			}
			else if(map[i][j].isMachine == 1)
			{
				if(map[i][j].modetype == 16)
					map[i][j].modetype =17;
				else if (map[i][j].modetype == 24)
					map[i][j].modetype = 25;
				else if (map[i][j].modetype == 17)
				{
					map[i][j].modetype = 0;
					map[i][j].isMachine = 0;
				}
				else if (map[i][j].modetype == 25)
				{
					map[i][j].modetype = 0;
					map[i][j].isMachine = 0;
				}
			}
		}
	}
	return;
}

Product::Product()
{
	//isLive = 0;
	moveRatio = 0;
	moveBegin = Point();
	moveEnd = Point();

}
Product::Product(PipelineUnit beginUnit)
{
	if (beginUnit.type > 0)	isPipeLine = 1;
	else isPipeLine = 0;
	isMachine = beginUnit.isMachine;
	double px, py, pz;
	if (beginUnit.type == 1 || beginUnit.type == 11 || beginUnit.type == 13 || beginUnit.type == 17)
	{
		direction = 1;
	}
	else if (beginUnit.type == 2 || beginUnit.type == 18 || beginUnit.type == 19 || beginUnit.type == 20)
	{
		direction = 2;
	}
	else if (beginUnit.type == 3 || beginUnit.type == 9 || beginUnit.type == 12 || beginUnit.type == 14)
	{
		direction = 3;
	}
	else if (beginUnit.type == 4 || beginUnit.type == 10 || beginUnit.type == 21 || beginUnit.type == 22)
	{
		direction = 4;
	}
	else if (beginUnit.type > 4 && beginUnit.type < 9)
	{
		direction = 0;
	}
	if (beginUnit.type == 1)//初始起点方向朝上
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 2)//初始起点方向朝left
	{
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 3)//初始起点方向朝down
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 4)//初始起点方向朝right
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 5)//流水线终点方向朝上
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 6)//流水线终点方向朝left
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 7)//流水线终点方向朝down
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 8)//流水线终点方向朝right
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}


	else if (beginUnit.type == 9)//普通流水线轨道up->down方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;

		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 10)//普通流水线轨道left->right方向
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 11)//11：普通流水线轨道left->up方向
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 12)//普通流水线轨道left->down方向
	{
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 13)//13: 普通流水线轨道right->up方向
	{
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 14)//14：普通流水线right->down方向
	{
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 17)//普通流水线轨道down->up方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 18)//普通流水线轨道right->left方向
	{
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);

	}
	else if (beginUnit.type == 19)//普通流水线轨道up->left方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);
		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 20)//普通流水线轨道down->left方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = beginUnit.leftbottom.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 21)//普通流水线轨道up->right方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.rightup.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);
		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}
	else if (beginUnit.type == 22)//普通流水线down->right方向
	{
		px = (beginUnit.leftbottom.x + beginUnit.rightup.x) / 2;
		py = beginUnit.leftbottom.y;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveBegin = Point(px, py, pz);

		px = beginUnit.rightup.x;
		py = (beginUnit.leftbottom.y + beginUnit.rightup.y) / 2;
		pz = std::max(beginUnit.leftbottom.z, beginUnit.rightup.z) + pipelineZline;
		moveEnd = Point(px, py, pz);
	}

	if (beginUnit.type > 0)isPipeLine = 1;
	moveRatio = 0;

}


void Product::drawProduct()
{
	//if (isMachine != 1)
	//{
		glPushMatrix();
		glTranslatef(moveBegin.x * (1 - moveRatio) + moveEnd.x * moveRatio, moveBegin.y * (1 - moveRatio) + moveEnd.y * moveRatio, moveBegin.z * (1 - moveRatio) + moveEnd.z * moveRatio+100);
		//glColor3f(1.0, 1.0, 0.0);
		//glScalef(200, 400, 200);
		//glTranslatef();
		//glutSolidCube(120);
		glCallList(view_list[4]);
		//if (moveRatio < 1)moveRatio += movespeed;
		//if (moveRatio >= 1)
		//{
		//	isLive = 0;
		//	hasPaint = 1;
		//}
		glPopMatrix();
		moveRatio += movespeed;
	//}
	//if (isMachine == 1)moveRatio += movespeed;
}

ProductMap::ProductMap(PipelineMap pipemap)
{
	hasPaint = 0;//初始化为没有开始绘制
	BeginPoint.resize(2, -1);
	nowEndPoint.resize(2, -1);
	nowPaint.resize(2, -1);
	this->BeginPoint = pipemap.BeginPoint;
	this->nowEndPoint = pipemap.BeginPoint;
	showmode = 0;


	for (int i = 0; i < PIPESIZE; i++)
	{
		for (int j = 0; j < PIPESIZE; j++)
		{
			this->map[i][j] = Product(pipemap.map[i][j]);
		}
	}

}

void ProductMap::drawMap()
{

	if (showmode == 1)
	{
		//printf("in showmode");
		if (nowPaint[0] == -1 && nowPaint[1] == -1)
		{
			nowPaint = BeginPoint;
		}
		if (map[nowPaint[0]][nowPaint[1]].moveRatio > 1)
		{
			map[nowPaint[0]][nowPaint[1]].moveRatio = 0;
			if (map[nowPaint[0]][nowPaint[1]].direction == 1)
			{
				nowPaint[1] += 1;
			}
			else if (map[nowPaint[0]][nowPaint[1]].direction == 2)
			{
				nowPaint[0] -= 1;
			}
			else if (map[nowPaint[0]][nowPaint[1]].direction == 3)
			{
				nowPaint[1] -= 1;
			}
			else if (map[nowPaint[0]][nowPaint[1]].direction == 4)
			{
				nowPaint[0] += 1;
			}
			else if (map[nowPaint[0]][nowPaint[1]].direction == 0 )
			{
				nowPaint = BeginPoint;
			}
		}
		map[nowPaint[0]][nowPaint[1]].drawProduct();

	}
}

ProductMap::ProductMap()
{
	hasPaint = 0;//初始化为没有开始绘制
	BeginPoint.resize(2, -1);
	nowEndPoint.resize(2, -1);
	showmode = 0;
}