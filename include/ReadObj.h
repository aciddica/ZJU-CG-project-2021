#ifndef _READOBJ_H_
#define _READOBJ_H_
#include<windows.h>
#include <GLFW\glfw3.h>
#include<vector>
#include<map>
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define M_PI 3.14159
#define BITMAP_ID 0x4D42
using namespace std;

struct Float3       //点的位置信息和法线信息的数据类型
{
	float Data[3];     //x,y,z
};
struct Float2      //点的纹理坐标数据类型
{
	float Data[2];   //u,v
};
struct Face          //面信息
{
	int vertex[4][3];       //三个点构成一个面  每个点有三个索引信息
};
struct Material
{
	float Data[4][3];
	string Kd_pic = "";
	GLuint Kd_pic_flag = -1;
};

/*
	.obj文件内的单个物体：【（面片信息）+纹理贴图】集合。
*/
class objModel
{
public:
	void push_face(Face* f)
	{
		mFace.push_back(*f);
	}
	void clear_faceVector() {
		int len = mFace.size();
		for (int i = 0; i < len; i++) {
			mFace.pop_back();
		}
	}
	void set_nameString(string* s)
	{
		Material_nameString = *s;
	}
	vector<Face>* get_mFace() {
		return &mFace;
	}
	string* get_nameString() {
		return &Material_nameString;
	}
private:
	vector<Face> mFace;              //三角面片的集合
	string Material_nameString;      //材质索引字符串
};
/*
	整个.obj文件：【（所有顶点信息+法线信息+纹理坐标信息）+ 物体】的集合。
*/
class objFile
{
public:
	objFile(const char* objFileName);//构造objFile对象
	void objFileDraw();              //画出obj文件的所有物体
private:
	vector<Float3> mLocation;        //所有点的信息
	vector<Float3> mNormal;          //所有法线信息
	vector<Float2> mTexcoord;        //所有纹理坐标信息
	vector<objModel> objs;           //所有的物体
	map<string, Material> mtlMap;   //与.obj相关的.mtl文件的所有材质信息【后续可以添加纹理信息，只要拓展Material即可】
};

/*
	文件读取
*/
unsigned char* LoadFileContent(const char* path, int& filesize);
unsigned char* LoadBitmapFile(char* filename, BITMAPINFOHEADER* bitmapInfoHeader);
void texload(int i, char* filename);

#endif