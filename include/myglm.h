/*Add by KQ*/
#ifndef _MYGLM_H_
#define _MYGLM_H_
/*end*/
#include <math.h>
#include <vector>
#include<iostream>
#include <GLFW\glfw3.h>
#include<stdio.h>
#define MAPSIZE  12
#define MAPBOUND 3000.0
#define GRIDLEN (MAPBOUND / MAPSIZE)
#define PIPE_GRID_TIME 2 //pipeline的一个小块是地图小块的几倍长
#define PIPESIZE (MAPSIZE/PIPE_GRID_TIME)
extern double CameraPosition[];
extern double CameraDirection[];
extern double TargetDistance;
/*add by KQ*/
extern int view_list[];
extern int top;
/*end*/
void Stage();
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



class PipelineUnit//流水线单元
{
public:
	Point leftbottom;
	Point rightup;
	int isMachine = 0;//是否是中间的处理机器
	double Len = PIPE_GRID_TIME * GRIDLEN;//每个流水线零件单元格的长度

	int type;
	int modetype;
	//流水线的部件分类 在流水线数组中方便根据不同的数字绘制不同的流水线组件 
	//0：没有流水线 1：流水线起点 朝向up 2：流水线起点 朝向left 3：流水线起点 朝向down 4：流水线起点 朝向right
	//5：流水线终点 朝向up 6：流水线终点 朝向left 7：流水线终点 朝向down 8：流水线终点 朝向right 
	//9：普通流水线轨道up->down方向  10: 普通流水线轨道left->right方向 11：普通流水线轨道left->up方向
	//12：普通流水线轨道left->down方向 13: 普通流水线轨道right->up方向 14：普通流水线right->down方向
	//15：初始的起点  
	//16 某些流水线上的处理装置。
	//17：普通流水线轨道down->up方向  18: 普通流水线轨道right->left方向 19：普通流水线轨道up->left方向
	//20：普通流水线轨道down->left方向 21: 普通流水线轨道up->right方向 22：普通流水线down->right方向
	//23及以上是其他流水线上的处理装置

	PipelineUnit();
	PipelineUnit(int s, Point lu, Point rb);
	void ChangeType(int s);
	void drawModel();//根据不同的type绘制不同的流水线组成部分
};

class Product//被操作的工件
{
public:
	//int isLive = 0;//这个工件是否存在
	int isPipeLine = 0;//这个地方是否有流水线单元
	int isMachine = 0;//这个流水线单元是否是加工器
	double moveRatio = 0;//工件在某个流水线单元上运动完成的百分比，例如50%是走了一半
	double movespeed = 0.1;//每次移动移动的ratio量，相当于速度
	double pipelineZline = 0.3;//流水线的厚度，防止工件和流水线重叠。
	double large = 0.2;//画的货物的大小
	int hasPaint = 0;//是否被画过了
	int direction;//工件的下一个位置在当前位置的哪个方向？0：没有下一个位置
	//1：up 2：left 3：down 4：right
	Point moveBegin, moveEnd;//在这个流水线单元上的运动起始点和结束点

	Product();
	Product(PipelineUnit beginUnit);//用某个流水线节点初始化工件。

	//void MoveProduct();//工件不断改变位置
	void drawProduct();//画出工件

};

class PipelineMap//流水线地图
{
public:
	double Len = PIPE_GRID_TIME * GRIDLEN;
	PipelineUnit map[PIPESIZE][PIPESIZE];

	int haveBegin = 0;//是否有了起点
	std::vector<int> BeginPoint;//起点的坐标，方便之后物体向终点运动
	int isEnd = 0;//流水线是否画完了

	std::vector<int> nowEndPoint;
	//若未画完，则是画的流水线暂时末端的坐标；若已画完，则是流水线最终终点的坐标，方便之后物体向终点运动。

	int paintMode = 0;//绘制模式，若为0，则代表不再绘画，若为1，则代表可以从暂时的流水线末端向鼠标点击的点延申流水线
	//若为2，则可以将流水线上已经存在的普通流水线轨道改为某种加工器件。
	//若为3，则绘制结束，无法再改变流水线。
	int showmode = 0; //0则只绘制流水线，1会绘制流水线上运动的物体
	PipelineMap();
	void drawMap();
	void Select(double x, double y);
};

class ProductMap//物品地图
{
public:
	double Len = PIPE_GRID_TIME * GRIDLEN;
	Product map[PIPESIZE][PIPESIZE];

	std::vector<int> BeginPoint;//起点的坐标，方便之后物体向终点运动
	std::vector<int> nowEndPoint;//物品终点坐标
	std::vector<int> nowPaint;//正在绘制的物品的坐标
	int hasPaint = 0;
	int showmode = 0; //0则只绘制流水线，1会绘制流水线上运动的物体
	//PipeMap();
	ProductMap();
	ProductMap(PipelineMap pipemap);
	void drawMap();
};
#endif