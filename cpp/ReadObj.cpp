#include <iostream>
#include <sstream>
#include <string>
#include "ReadObj.h"
using namespace std;
int numOfFaces = 0;
/*for test*/
int cnt_faces = 0;
/*end*/
unsigned int texture[100];
unsigned char* LoadFileContent(const char* path, int& filesize)   //该函数来自于萌谷教程
//读取文件，返回文件内容，把文件大小赋值给filesize       
{
	unsigned char* fileContent = nullptr;
	filesize = 0;
	FILE* pFile = NULL;
	errno_t err;
	err = fopen_s(&pFile, path, "rb");
	if (err) cout << "obj/mtl文件打开失败！";
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);      //将文件指针移动到文件末尾
		int nLen = ftell(pFile);        //距离文件头部的距离   //这里指文件大小
		if (nLen > 0)
		{
			rewind(pFile);          //移动到文件头部
			fileContent = new unsigned char[nLen + 1];
			//为文件指针开辟空间
			fread(fileContent, sizeof(unsigned char), nLen, pFile);
			//将pFile的内容读入fileContent
			fileContent[nLen] = '\0';         //文件末尾加上\0
			filesize = nLen;                  //为文件大小赋值
		}
		fclose(pFile);
	}
	return fileContent;
}
unsigned char* LoadBitmapFile(char* filename, BITMAPINFOHEADER* bitmapInfoHeader)
{
	FILE* filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char* bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量

								// 以“二进制+读”模式打开文件filename 
	fopen_s(&filePtr, filename, "rb");
	if (filePtr == NULL) return NULL;
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}
void texload(int i, char* filename)
{

	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap信息头
	unsigned char* bitmapData;                                       // 纹理数据

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap层次(通常为0，表示最上层) 
		GL_RGB,	//我们希望该纹理有红、绿、蓝数据
		bitmapInfoHeader.biWidth, //纹理宽带，必须是n，若有边框+2 
		bitmapInfoHeader.biHeight, //纹理高度，必须是n，若有边框+2 
		0, //边框(0=无边框, 1=有边框) 
		GL_RGB,	//bitmap数据的格式
		GL_UNSIGNED_BYTE, //每个颜色数据的类型
		bitmapData);	//bitmap数据指针  
}
objFile::objFile(const char* objFileName)
{
	int nFileSize = 0;
	unsigned char* objFileContent = LoadFileContent(objFileName, nFileSize);
	if (objFileContent == nullptr) {
		return;
	}
	stringstream ssobjFileContent((char*)objFileContent);//获得整个文件的字符串流
	string head = "";
	int cnt_g = 0;
	objModel tempobjModel;//物体模型对象，每到g或者文件末尾的时候打包送至objs
	char szoneLine[256];//一行的字符串
	while (!ssobjFileContent.eof())
	{
		memset(szoneLine, 0, 256);//每次循环初始化数组
		ssobjFileContent.getline(szoneLine, 256, '\n');//从流读取一行
		if (strlen(szoneLine) > 0)
		{
			stringstream ssOneLine(szoneLine);//一行转化为流
			ssOneLine >> head;//读取头!!!如果本行无法读取头则什么也不做，就是保持之前的状态。
			//cout << "head==" << head << endl;
			//cout << "szoneLine==  ";
			//cout << szoneLine << endl;
			if (head == "mtllib") {//指定的mtl文件
				string mtlFileName;
				ssOneLine >> mtlFileName;
				char* p = &mtlFileName[0];

				/*handle_mtlFile(p);*///解析obj指定的mtl文件
				int mtlFileSize = 0;
				unsigned char* mtlFileContent = LoadFileContent(p, mtlFileSize);//打开.mtl文件并解析
				//cout << mtlFileContent << endl;
				if (mtlFileContent == nullptr)
				{
					head = "";
					continue;
				}
				stringstream ssmtl((char*)mtlFileContent);
				char szoneLine_mtl[256];
				string key;//材质的名称
				while (!ssmtl.eof())
				{
					memset(szoneLine_mtl, 0, 256);        //  每次循环初始化数组szoneLine
					ssmtl.getline(szoneLine_mtl, 256);      //流读取一行
					if (strlen(szoneLine_mtl) > 0)       //该行不为空
					{
						stringstream ssOneLine_mtl(szoneLine_mtl);
						string head_mtl;
						ssOneLine_mtl >> head_mtl;
						if (head_mtl == "newmtl") {
							ssOneLine_mtl >> key;
						}
						else if (head_mtl == "Ka") {
							ssOneLine_mtl >> mtlMap[key].Data[0][0] >> mtlMap[key].Data[0][1] >> mtlMap[key].Data[0][2];
						}
						else if (head_mtl == "Kd") {
							ssOneLine_mtl >> mtlMap[key].Data[1][0] >> mtlMap[key].Data[1][1] >> mtlMap[key].Data[1][2];
						}
						else if (head_mtl == "Ks") {
							ssOneLine_mtl >> mtlMap[key].Data[2][0] >> mtlMap[key].Data[2][1] >> mtlMap[key].Data[2][2];
						}
						else if (head_mtl == "Ns") {
							ssOneLine_mtl >> mtlMap[key].Data[3][0];
						}
						else if (head_mtl == "map_Kd") {
							ssOneLine_mtl >> mtlMap[key].Kd_pic;
						}
						else if (head_mtl == "#") {
							continue;
						}
					}
				}
				/*绑定纹理*/
				//1.纹理计数
				int cnt_texture = 0;
				for (auto p = mtlMap.begin(); p != mtlMap.end(); p++)
				{
					if (p->second.Kd_pic != "") {
						cnt_texture++;
					}
				}
				glGenTextures(cnt_texture, texture);//产生数量一致的texture
				int cnt_loadtexture = 0;
				for (auto p = mtlMap.begin(); p != mtlMap.end(); p++) {
					if (p->second.Kd_pic != "") {
						/*载入贴图*/
						p->second.Kd_pic_flag = cnt_loadtexture;
						char* q = &(p->second.Kd_pic[0]);
						texload(cnt_loadtexture++, q);
					}
				}
				delete mtlFileContent;

			}
			else if (head == "v") {//顶点信息
				Float3 tempLocation;
				ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
				mLocation.push_back(tempLocation);
			}
			else if (head == "vn") {//法线信息
				Float3 tempNormal;
				ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
				mNormal.push_back(tempNormal);
			}
			else if (head == "vt") {//纹理坐标信息
				Float2 tempTexcoord;
				ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];
				mTexcoord.push_back(tempTexcoord);
			}
			else if (head == "usemtl") {//usemtl之后有新的物体出现
				string tempnameString;
				ssOneLine >> tempnameString;
				tempobjModel.set_nameString(&tempnameString);//设置对应于mtl好材质的名称
			}
			else if (head == "g") {
				if (cnt_g) {
					objs.push_back(tempobjModel);
					tempobjModel.clear_faceVector();//清空tempobjModel的vector
				}
				cnt_g++;
			}
			else if (head == "f") {
				cnt_faces++;
				//cout << "length="<<strlen(szoneLine) << endl;
				string vertexStr;   //接收流的内容
				Face tempFace;      //面片结构

				/*判断是三角面片还是四边形面片*/
				int cnt_blank = 0;
				for (int k = 0; szoneLine[k] != '\0'; k++) {
					if (szoneLine[k] == ' ') cnt_blank++;
				}
				//cout << "cnt_blank==" << cnt_blank << "  ";
				//cout << "szoneLine=="<< szoneLine <<endl;
				numOfFaces = cnt_blank;
				if (numOfFaces == 3) {
					for (int i = 0; i < 3; ++i)         //每个面三个点
					{
						ssOneLine >> vertexStr;           //从流中读取点的索引信息
						size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
						string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
						size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
						string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息!!!!【注意是纹理坐标在中间!!!】
						string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息
						tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //法线坐标索引信息赋值
						tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //纹理信息赋值
						tempFace.vertex[i][0] = atoi(locIndexStr.c_str());          //将索引信息从 srting转换为 int     //位置索引信息赋值


					}
					tempobjModel.push_face(&tempFace);
				}
				else if (numOfFaces == 4) {
					for (int i = 0; i < 4; ++i)         //每个面三个点
					{
						ssOneLine >> vertexStr;           //从流中读取点的索引信息
						size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
						string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
						size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
						string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息
						string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息
						tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //法线信息赋值
						tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //纹理坐标索引信息赋值
						tempFace.vertex[i][0] = atoi(locIndexStr.c_str());          //将索引信息从 srting转换为 int     //位置索引信息赋值
					}
					tempobjModel.push_face(&tempFace);
				}

			}
			else if (head == "#") {
				head = "";
				continue;
			}
			head = "";
		}
	}
	objs.push_back(tempobjModel);
	delete objFileContent;
	cout << "mLocationSize= " << mLocation.size() << endl;
	cout << mLocation[0].Data[0] << " " << mLocation[0].Data[1] << " " << mLocation[0].Data[2] << endl;
	cout << "mNormalSize= " << mNormal.size() << endl;
	cout << mNormal[0].Data[0] << " " << mNormal[0].Data[1] << " " << mNormal[0].Data[2] << endl;
	cout << "mTexcoordsize= " << mTexcoord.size() << endl;
	cout << mTexcoord[0].Data[0] << " " << mTexcoord[0].Data[1] << " " << mTexcoord[0].Data[2] << endl;
	cout << "cnt_faces=" << cnt_faces << endl;
	cout << "face[0]" << objs[0].get_mFace()->begin()->vertex[0][0] << " " << objs[0].get_mFace()->begin()->vertex[0][1] << " " << objs[0].get_mFace()->begin()->vertex[0][2] << endl;
	cout << "face[0]" << objs[0].get_mFace()->begin()->vertex[1][0] << " " << objs[0].get_mFace()->begin()->vertex[1][1] << " " << objs[0].get_mFace()->begin()->vertex[1][2] << endl;
	cout << "face[0]" << objs[0].get_mFace()->begin()->vertex[2][0] << " " << objs[0].get_mFace()->begin()->vertex[2][1] << " " << objs[0].get_mFace()->begin()->vertex[2][2] << endl;
}
void objFile::objFileDraw()
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	for (int i = 0; i < objs.size(); i++)
	{
		/*打开纹理映射*/
		if (mtlMap[(*(objs[i].get_nameString()))].Kd_pic != "") {
			GLuint texture_enum = mtlMap[(*(objs[i].get_nameString()))].Kd_pic_flag;
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture[texture_enum]);
		}
		/*设置材质*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, mtlMap[(*(objs[i].get_nameString()))].Data[0]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mtlMap[(*(objs[i].get_nameString()))].Data[1]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mtlMap[(*(objs[i].get_nameString()))].Data[2]);
		glMaterialfv(GL_FRONT, GL_SHININESS, mtlMap[(*(objs[i].get_nameString()))].Data[3]);
		//cout << mtlMap[(*(objs[i].get_nameString()))].Data[0][0] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[0][1] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[0][2] << endl;
		//cout << mtlMap[(*(objs[i].get_nameString()))].Data[1][0] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[1][1] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[1][2] << endl;
		//cout << mtlMap[(*(objs[i].get_nameString()))].Data[2][0] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[2][1] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[2][2] << endl;
		//cout << mtlMap[(*(objs[i].get_nameString()))].Data[3][0] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[3][1] << " " << mtlMap[(*(objs[i].get_nameString()))].Data[3][2] << endl;

		/*描点画图*/
		if (numOfFaces == 3) {
			glBegin(GL_TRIANGLES);
			for (auto faceIndex = objs[i].get_mFace()->begin(); faceIndex != objs[i].get_mFace()->end(); ++faceIndex)
			{
				//第一个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[0][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[0][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[0][0] - 1].Data);
				//第二个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[1][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[1][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[1][0] - 1].Data);
				//第三个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[2][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[2][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[2][0] - 1].Data);
			}
			glEnd();
		}
		else if (numOfFaces == 4) {
			glBegin(GL_QUADS);
			for (auto faceIndex = objs[i].get_mFace()->begin(); faceIndex != objs[i].get_mFace()->end(); ++faceIndex)
			{
				//第一个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[0][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[0][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[0][0] - 1].Data);
				//第二个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[1][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[1][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[1][0] - 1].Data);
				//第三个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[2][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[2][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[2][0] - 1].Data);
				//第四个点的法线，纹理，位置信息
				glNormal3fv(mNormal[faceIndex->vertex[3][2] - 1].Data);
				glTexCoord2fv(mTexcoord[faceIndex->vertex[3][1] - 1].Data);
				glVertex3fv(mLocation[faceIndex->vertex[3][0] - 1].Data);
			}
			glEnd();
		}
		/*关闭纹理映射*/
		glDisable(GL_TEXTURE_2D);
	}
}
