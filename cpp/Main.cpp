

#include "main.h"
#include <stdlib.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#pragma warning(disable : 4996)
#define glfw_window_height 720
#define glfw_window_wide  720
/*end*/
#pragma comment(lib,"glew32.lib")



void renewCamera()
{
	CameraPosition[0] = 0;
	CameraPosition[1] = 0;
	CameraPosition[2] = CameraSwitch ? InitialRoamCameraPos : InitialCameraPos;
	CameraDirection[0] = 0;
	CameraDirection[1] = 0;
	CameraDirection[2] = -1;
	up[0] = 0;
	up[1] = 1;
	up[2] = 0;
}
//added by wys
void draw(GLboolean shadowRender)
{
	GLboolean textureOn = glIsEnabled(GL_TEXTURE_2D);

	if (shadowRender)
		glDisable(GL_TEXTURE_2D);

	gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraPosition[0] + CameraDirection[0] * TargetDistance, CameraPosition[1] + CameraDirection[1] * TargetDistance, CameraPosition[2] + CameraDirection[2] * TargetDistance, 0.0, 1.0, 0.0);

	/*add by KQ*/
	glPushMatrix();
	//pmq
	glRotatef(90, -1, 0, 0);
	glTranslatef(0, -6, -1);
	BackGroundScene.drawMap();
	PipeLine.drawMap();

	glPushMatrix();
	//glCallList(view_list[1]);//draw stage
	Stage();
	glPopMatrix();
	glPopMatrix();

	if (shadowRender && textureOn)
		glEnable(GL_TEXTURE_2D);
}
/*初始化光源与阴影图*/
void lightinit(void)
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color[index]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color[index]);
	glLightfv(GL_LIGHT0, GL_SHININESS, shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color[index]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, color[index]);
	glLightfv(GL_LIGHT1, GL_SHININESS, shininess);
	glLightfv(GL_LIGHT1, GL_POSITION, position2);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
}
void transposeMatrix(GLfloat m[16])
{
	GLfloat  tmp;
#define Swap( a, b )    tmp = a; a = b; b = tmp
	Swap(m[1], m[4]);
	Swap(m[2], m[8]);
	Swap(m[3], m[12]);
	Swap(m[6], m[9]);
	Swap(m[7], m[13]);
	Swap(m[11], m[14]);
#undef Swap
}
/*生成阴影图*/
void generateShadowMap(void)
{
	GLint    viewport[4];
	GLfloat  lightPos[4];

	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);//获得光源的位置
	glGetIntegerv(GL_VIEWPORT, viewport);//获得视口信息

	glViewport(0, 0, 256, 256);//设置视口与阴影图大小匹配

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(80.0, 1.0, 10.0, 10000.0);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(lightPos[0], lightPos[1], lightPos[2],
		lookat[0], lookat[1], lookat[2],
		Up[0], Up[1], Up[2]);//把摄像机放到光源处

	draw(GL_TRUE);//绘制场景

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 256, 256, 0);//摄像机移到光源处渲染的阴影图

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);//恢复当前视口

	//绘制阴影图
	if (showShadow) {
		GLfloat depthImage[256][256];
		glReadPixels(0, 0, 256, 256, GL_DEPTH_COMPONENT, GL_FLOAT, depthImage);
		glWindowPos2f(viewport[2] / 2, 0);//设置阴影图绘制位置
		glDrawPixels(256, 256, GL_LUMINANCE,
			GL_FLOAT, depthImage);
		//glutSwapBuffers();
	}
}
//自动生成阴影图的纹理坐标
void generateTextureMatrix(void)
{
	GLfloat  tmpMatrix[16];

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.0);
	glScalef(0.5, 0.5, 1.0);
	gluPerspective(60.0, 1.0, 1.0, 10000.0);
	gluLookAt(lightPos[0], lightPos[1], lightPos[2],
		lookat[0], lookat[1], lookat[2],
		Up[0], Up[1], Up[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, tmpMatrix);
	glPopMatrix();

	transposeMatrix(tmpMatrix);

	glTexGenfv(GL_S, GL_OBJECT_PLANE, &tmpMatrix[0]);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, &tmpMatrix[4]);
	glTexGenfv(GL_R, GL_OBJECT_PLANE, &tmpMatrix[8]);
	glTexGenfv(GL_Q, GL_OBJECT_PLANE, &tmpMatrix[12]);
}
//end
void grab(void)
{
	FILE* pDummyFile;  //指向另一bmp文件，用于复制它的文件头和信息头数据
	FILE* pWritingFile;  //指向要保存截图的bmp文件
	GLubyte* pPixelData;    //指向新的空的内存，用于保存截图bmp文件数据
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;   //BMP文件数据总长度

	// 计算像素数据的实际长度
	i = WindowWidth * 3;   // 得到每一行的像素数据长度
	while (i % 4 != 0)      // 补充数据，直到i是的倍数
		++i;
	PixelDataLength = i * WindowHeight;  //补齐后的总位数

	// 分配内存和打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);


	pDummyFile = fopen("img/bmp_example.bmp", "rb");//只读形式打开
	if (pDummyFile == 0)
		exit(0);

	string num = to_string(pic_num);
	string filename = "img/pic" + num + ".bmp";

	pWritingFile = fopen(filename.c_str(), "wb"); //只写形式打开
	if (pWritingFile == 0)
		exit(0);

	//把读入的bmp文件的文件头和信息头数据复制，并修改宽高数据
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);  //读取文件头和信息头，占据54字节
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET); //移动到0X0012处，指向图像宽度所在内存
	i = WindowWidth;
	j = WindowHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 读取当前画板上图像的像素数据
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadPixels(0, 0, WindowWidth, WindowHeight, GL_BGR, GL_UNSIGNED_BYTE, pPixelData);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	//把完整的BMP文件数据写入pWritingFile
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
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
	double ClickDirection[3];
	ClickPosition[0] = objx; ClickPosition[1] = objy; ClickPosition[2] = objz;

	ClickDirection[0] = ClickPosition[0] - CameraPosition[0];
	ClickDirection[1] = ClickPosition[1] - CameraPosition[1];
	ClickDirection[2] = ClickPosition[2] - CameraPosition[2];

	if (ClickDirection[2] == 0) return; //parallel to xoy
	normalize(ClickDirection);
	double len = (-1 * CameraPosition[2]) / ClickDirection[2];
	printf("select: x=%lf,y=%lf\n", CameraPosition[0] + len * ClickDirection[0], CameraPosition[1] + len * ClickDirection[1]);
	BackGroundScene.Select(CameraPosition[0] + len * ClickDirection[0], CameraPosition[1] + len * ClickDirection[1], 1);
	PipeLine.Select(CameraPosition[0] + len * ClickDirection[0], CameraPosition[1] + len * ClickDirection[1]);
}
static bool BlockCheck(double newx, double newy, double newz)
{
	int i = -1;
	int j = -1;
	double Len = PIPE_GRID_TIME * GRIDLEN;
	printf("CameraSwitch=%d\n", CameraSwitch);
	printf("newz=%lf\n", newz);
	if (!CameraSwitch) return 0;//only in the view mode enables movement block check 
	if (newx > RoamLimit || -newz + 0.5 * MAPBOUND > RoamLimit || newx < -RoamLimit || -newz + 0.5 * MAPBOUND < -RoamLimit || newy < BottomLimit || newy > HeightLimit) //out of boundary
	{
		return 1;
		printf("boundary block");
	}
	else //check if there exists a pipeline item blocks the way
	{
		i = (newx + 0.5 * MAPBOUND) / Len;
		j = (-newz + 0.5 * MAPBOUND) / Len;
		if (i >= 0 && j >= 0 && i < PIPESIZE && j < PIPESIZE && PipeLine.map[i][j].type != 0 && newy < PipelineComponetHeight) return 1;
		else return 0;
	}

}
void init()
{

	/*add by KQ*/
	/*��ʾ�б�����ˮ��*/
	GLint lid = glGenLists(1);
	glNewList(lid, GL_COMPILE);
	glPushMatrix();
	view_list[top++] = lid;
	objFile obj("belt2_usable.obj");
	//glScalef(180, 360, 120);
	//glScalef(180, 720, 120);
	//objFile obj("belt.obj");
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0.3 * 180, 2.0 * 360, -1.3 * 120);
	//glTranslatef(0.0, -1.8 * 360, 0.0);
	glTranslatef(0.0, -1.8 * 360, 0.0);
	glTranslatef(90.0, 0.0, 0.0);
	obj.objFileDraw();
	glPopMatrix();
	glEndList();

	/*��ʾ�б�������*/
	GLint lid1 = glGenLists(1);
	glNewList(lid1, GL_COMPILE);
	glPushMatrix();
	view_list[top++] = lid1;
	objFile obj1("factory_usable.obj");
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0.0, 5000);
	glTranslatef(-3000, 0.0, 0);
	//glScalef(7, 7, 7);
	obj1.objFileDraw();
	glPopMatrix();
	glEndList();
	/*end*/

	/*added by liuyifei */
	GLint lid2 = glGenLists(1);
	glNewList(lid2, GL_COMPILE);
	glPushMatrix();
	view_list[top++] = lid2;
	objFile obj2("robot_arm2_usable.obj");
	//objFile obj2("robotic_arm.obj");
	glTranslatef(0, 0.1, 0.2);
	//glScalef(20, 10, 20);
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 100, 0.0);
	glTranslatef(-100, 0, 0.0);
	obj2.objFileDraw();
	glPopMatrix();
	glEndList();


	GLint lid3 = glGenLists(1);
	glNewList(lid3, GL_COMPILE);
	glPushMatrix();
	view_list[top++] = lid3;
	objFile obj3("robotic_arm.obj");
	glTranslatef(0, 0.1, 0.2);
	//glScalef(0.1, 0.1, 0.1);
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 100.0, 0.0);
	glTranslatef(-50.0, 0.0, -0.0);
	obj3.objFileDraw();
	glPopMatrix();
	glEndList();

	GLint lid4 = glGenLists(1);
	glNewList(lid4, GL_COMPILE);
	glPushMatrix();
	view_list[top++] = lid4;
	objFile obj4("cube2.obj");
	//glScalef(1.0, 1.0, 0.7);
	glTranslatef(0.0, -50.0, 0.0);
	obj4.objFileDraw();
	glPopMatrix();
	glEndList();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//如下为glfw函数
double mouse_x = 0;
double mouse_y = 0;


float C_M_X = 0;
float C_M_Y = 0;
float C_M_Z = InitialCameraPos;
float C_L_X = 0;
float C_L_Y = 0;
float C_L_Z = -1;
float L_P_X =-1;
float L_P_Y = 1;
float L_P_Z = -1;

bool cameral_move_window = false;
bool cameral_lookat_window = false;
bool light_window = false;

void framebuffer(GLFWwindow* window, int W, int H) {
	printf("W=%d,H=%d\n", W, H);
	glViewport(0, 0, W, H);
	reshape(W, H);
}

// 输入监听
void glfw_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	double x = mouse_x;
	double y = mouse_y;
	//printf("button:x=%d,y=%d,action=%d\n", x, y, action);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		CameraRotate = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action != GLFW_PRESS)
	{
		CameraRotate = false;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS & !CameraSwitch)
	{
		SelectMap(x, y);
	}
	//SelectMap(370, 330);
	//PipeLine.paintMode = 1;

}
void glfw_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	int k = key;
	//cout << k << endl;
	if (k >= 65 && k <= 90) {
		k = k + 32;
	}
	//cout << k << endl;
	static bool hasInitialProduct = false;
	double CameraStep = 10.0;
	double CameraRight[3];
	cross(up, CameraDirection, CameraRight);
	//cout <<"right " << CameraRight[0] << " " << CameraRight[1] << " " << CameraRight[2] << endl;
	switch (k)
	{
	case 27:
	case 'q': if (action == GLFW_PRESS) {exit(0); break; }
	case 'w':
	{
		double newx = CameraPosition[0] + CameraDirection[0] * CameraStep;
		double newy = CameraPosition[1] + CameraDirection[1] * CameraStep;
		double newz = CameraPosition[2] + CameraDirection[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz); //block the movement		
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
		printf("[block debug]:  next pos-(%lf,%lf,%lf)  block status:%d \n", newx, newy, newz, Blocked);
	} break;
	case 's':
	{
		double newx = CameraPosition[0] - CameraDirection[0] * CameraStep;
		double newy = CameraPosition[1] - CameraDirection[1] * CameraStep;
		double newz = CameraPosition[2] - CameraDirection[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz); //block the movement		
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
		printf("[block debug]:  next pos-(%lf,%lf,%lf)  block status:%d \n", newx, newy, newz, Blocked);
	} break;
	case 'a':
	{
		double newx = CameraPosition[0] + CameraRight[0] * CameraStep;
		double newy = CameraPosition[1] + CameraRight[1] * CameraStep;
		double newz = CameraPosition[2] + CameraRight[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz); //block the movement		
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
	} break;
	case 'd':
	{
		double newx = CameraPosition[0] - CameraRight[0] * CameraStep;
		double newy = CameraPosition[1] - CameraRight[1] * CameraStep;
		double newz = CameraPosition[2] - CameraRight[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz);
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
	} break;
	case ' ':
	{
		double newx = CameraPosition[0] + up[0] * CameraStep;
		double newy = CameraPosition[1] + up[1] * CameraStep;
		double newz = CameraPosition[2] + up[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz);
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
	} break;
	case 'x':
	{
		double newx = CameraPosition[0] - up[0] * CameraStep;
		double newy = CameraPosition[1] - up[1] * CameraStep;
		double newz = CameraPosition[2] - up[2] * CameraStep;
		bool Blocked = BlockCheck(newx, newy, newz);
		if (!Blocked)
		{
			CameraPosition[0] = newx;
			CameraPosition[1] = newy;
			CameraPosition[2] = newz;
		}
	}break;
	case 'p':
		if (action == GLFW_PRESS) {
		if (PipeLine.paintMode < 3)
			PipeLine.paintMode += 1;
		if (PipeLine.paintMode % 3 == 0)
		{
			PipeLine.isEnd = 1;
		}
	}break;
	case 'c':
		if (action == GLFW_PRESS) {
		CameraSwitch = !CameraSwitch;
		if (CameraSwitch) CameraPosition[2] = InitialRoamCameraPos;
		else CameraPosition[2] = InitialCameraPos;
		//renewCamera();
	}break;
	case '0':
		if (action == GLFW_PRESS) {
		renewCamera();
	}break;

	case 'o':
		if (action == GLFW_PRESS) {

		if (PipeLine.isEnd == 1)
		{
			if (!hasInitialProduct)
			{
				product = ProductMap(PipeLine);
				hasInitialProduct = !hasInitialProduct;
			}
			if (showProduct == 0)showProduct = 1;
			else showProduct = 0;
			product.showmode = showProduct;
		}
		break;
	case '.': if (action == GLFW_PRESS) {
		if (index < 3) {
			index++;
		}
		else {
			index = 0;
		}
	}break;
	case 'b': if (action == GLFW_PRESS) {
		grab();
		pic_num++;
		cout << "截图成功" << endl;
	} break;
	}
	}

}
void KEYInput(GLFWwindow* window) {
	glfwSetKeyCallback(window, glfw_key);
}
void display_fw()
{
	//printf("PipeLine.paintMode=%d\n", PipeLine.paintMode);
	//added by wys
	lightinit();
	GLfloat  radius = 30;

	generateShadowMap();
	//generateTextureMatrix();

	if (showShadow) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();

	if (!CameraSwitch) {
		gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraPosition[0] + CameraDirection[0] * TargetDistance, CameraPosition[1] + CameraDirection[1] * TargetDistance, CameraPosition[2] + CameraDirection[2] * TargetDistance, 0.0, 1.0, 0.0);
		//pmq
		BackGroundScene.drawMap();
		PipeLine.drawMap();
		if (showProduct)product.drawMap();
		//glCallList(view_list[1]);

	}
	else {
		gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], CameraPosition[0] + CameraDirection[0] * TargetDistance, CameraPosition[1] + CameraDirection[1] * TargetDistance, CameraPosition[2] + CameraDirection[2] * TargetDistance, 0.0, 1.0, 0.0);
		/*add by KQ*/
		glPushMatrix();
		//pmq
		glRotatef(90, -1, 0, 0);
		glTranslatef(0, -6, -1);
		BackGroundScene.drawMap();
		PipeLine.drawMap();
		if (showProduct)product.drawMap();
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, -1, 0, 0);
		glTranslatef(0, -6, -5);
		glTranslatef(-1.5 * GRIDLEN, InitialCameraPos - 6 * GRIDLEN, 0);
		Stage();
		glPopMatrix();
		//draw(GL_FALSE);
	}

	//glutSwapBuffers();
	//glutPostRedisplay();
}
void init_gui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
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
	gluPerspective(45.0f, whRatio, 1, 10000);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}
void glfw_mouse(GLFWwindow* window, double x, double y) {
	//printf("x=%lf,y=%lf\n", x, y);
	mouse_x = x;
	mouse_y = y;
	y = WindowHeight - y;
	static int LastX = 0;
	static int LastY = 0;
	static float CameraRotateX = 0;
	static float CameraRotateY = 0;
	if (CameraRotate)
	{
		if (x > LastX)
		{
			CameraRotateX -= RotateChange;
			LastX = x;
		}
		else
		{
			CameraRotateX += RotateChange;
			LastX = x;
		}
		if (y > LastY)
		{
			CameraRotateY -= RotateChange;
			LastY = y;
		}
		else
		{
			CameraRotateY += RotateChange;
			LastY = y;
		}
	}
	CameraDirection[0] = sin(CameraRotateX);
	CameraDirection[1] = sin(CameraRotateY);
	CameraDirection[2] = -cos(CameraRotateX);

	//printf("dir: %lf %lf %lf\n", CameraDirection[0], CameraDirection[1], CameraDirection[2]);
	normalize(CameraDirection);
}
void drawGUI() {
	// draw ui elements
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	const auto flags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_MenuBar;

	if (!ImGui::Begin("CG_FINAL_PIPELINE", nullptr, flags)) {
		ImGui::End();
	}
	else {
		if (ImGui::BeginMenuBar())
		{
			
			ImGui::EndMenuBar();
		}
		//begin drawing
		ImGui::Text("This is GUI.");    
		///////////开启子窗口
		if(ImGui::Button("Cameral move Window"))
			cameral_move_window = true;
		if(ImGui::Button("Cameral lookat Window"))
			cameral_lookat_window = true;
		if (ImGui::Button("light contorl Window"))
			light_window = true;

		//子窗口绘制
		if (cameral_move_window) {
			C_M_X = (float)CameraPosition[0];
			C_M_Y = (float)CameraPosition[1];
			C_M_Z = (float)CameraPosition[2];
			ImGui::Begin("cameral move window", &cameral_move_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("This is cameral move window!");
			ImGui::SliderFloat("X position", &C_M_X, -2000.0f, 2000.0f);
			ImGui::Text("Drag to modify the x position -2000 To 2000");
			ImGui::SliderFloat("Y position", &C_M_Y, -2000.0f, 2000.0f);
			ImGui::Text("Drag to modify the y position -2000 To 2000");
			ImGui::SliderFloat("Z position", &C_M_Z, -1000.0f, 10000.0f);
			ImGui::Text("Drag to modify the z position -1000 To 10000");

			CameraPosition[0] = (double)C_M_X;
			CameraPosition[1] = (double)C_M_Y;
			CameraPosition[2] = (double)C_M_Z;
			if (ImGui::Button("Close the window"))
				cameral_move_window = false;
			ImGui::End();
			

		}
		if (cameral_lookat_window) {
			C_L_X = (float)CameraDirection[0];
			C_L_Y = (float)CameraDirection[1];
			C_L_Z = (float)CameraDirection[2];
			ImGui::Begin("cameral look_at window", &cameral_lookat_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("This is cameral lookat window!");
			ImGui::SliderFloat("X Look at", &C_L_X, -1.0f, 1.0f);
			ImGui::Text("Drag to modify the x direction -1 To 1");
			ImGui::SliderFloat("Y Look at", &C_L_Y, -1.0f, 1.0f);
			ImGui::Text("Drag to modify the y direction -1 To 1");
			ImGui::SliderFloat("ZLook at", &C_L_Z, -1.0f, 1.0f);
			ImGui::Text("Drag to modify the y direction -1 To 1");

			CameraDirection[0] = (double)C_L_X;
			CameraDirection[1] = (double)C_L_Y;
			CameraDirection[2] = (double)C_L_Z;
			if (ImGui::Button("Close the window"))
				cameral_lookat_window = false;
			ImGui::End();


		}
		if (light_window) {
			L_P_X = position[0];
			L_P_Y = position[1];
			L_P_Z = position[2];
			ImGui::Begin("light position control window", &cameral_lookat_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("This is light position control window!");
			ImGui::SliderFloat("position X", &L_P_X, -2.0f, 0.0f);
			ImGui::Text("Drag to modify the light's x position -2 To 0");
			ImGui::SliderFloat("position Y", &L_P_Y, 0.0f, 2.0f);
			ImGui::Text("Drag to modify the light's y position 0 To 2");
			ImGui::SliderFloat("position Z", &L_P_Z, -2.0f, 0.0f);
			ImGui::Text("Drag to modify the light's y position -2 To 0");

			position[0] = L_P_X;
			position[1] = L_P_Y;
			position[2] = L_P_Z;
			if (ImGui::Button("Close the window"))
				light_window = false;
			ImGui::End();


		}



		///单一按键
		//for 'p'
		if (ImGui::Button("drawing mode")) {
			printf("PipeLine.paintMode=%d\n", PipeLine.paintMode);
			if (PipeLine.paintMode < 3)
				PipeLine.paintMode += 1;
			if (PipeLine.paintMode % 3 == 0)
			{
				PipeLine.isEnd = 1;
			}
		}
		ImGui::SameLine();
		ImGui::Text("PipeLine.paintMode = %d", PipeLine.paintMode);


		//for 'c'
		if (ImGui::Button("cameral switch")) {
			CameraSwitch = !CameraSwitch;
		}
		ImGui::SameLine();
		if (CameraSwitch == 1) {
			ImGui::Text("cameral is switched");
		}
		else {
			ImGui::Text("cameral is not switched");
		}
		//for '0'
		if (ImGui::Button("renew Camera")) {
			renewCamera();
		}
		ImGui::SameLine();
		if (CameraSwitch == 1) {
			ImGui::Text("cameral is renewed");
		}
		else {
			ImGui::Text("cameral is hold");
		}
		//for 'b'
		if (ImGui::Button("cut picture")) {
			grab();
			pic_num++;
			
		}
		ImGui::SameLine();
		

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//end drawing
		

		ImGui::End();
	}

	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
int main(int argc, char* argv[])
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(glfw_window_wide, glfw_window_height, "GLFW window for CG_FINAL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer);
	glfwSetCursorPosCallback(window, glfw_mouse);
	glfwSetMouseButtonCallback(window, glfw_mouse_button);
	init();
	
	glewInit();
	init_gui(window);
	reshape(glfw_window_wide, glfw_window_height);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//printf("f=%f,counter=%d\n", f, counter);//for testing
		/* Draw */
		KEYInput(window);
		display_fw();
		

		reshape(glfw_window_wide, glfw_window_height);
		drawGUI();
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		//检查是否出发相关事件(键盘,鼠标等)并调用相关的回调函数
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}