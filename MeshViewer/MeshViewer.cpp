#include <windows.h>
#include"stb_image.h"
#include <string.h>
#include <stdlib.h>
#include <gl\glui.h>
#include <math.h>
#include "Obj.h"
#include "common.h"
#include<cassert>
#include<ctime>

int g_xform_mode = TRANSFORM_NONE;
int   g_main_window;
double g_windows_width, g_windows_height;

//the lighting
static GLfloat g_light0_ambient[] = { 0.3f, 0.6f, 0.2f, 0.5f };//环境光
static GLfloat g_light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 0.5f };//散射光
static GLfloat g_light0_specular[] = { 0.0f,0.0f, 0.0f, 0.5f }; //镜面光
static GLfloat g_light0_position[] = { 0.0f, 0.0f, 30.0f, 0.5f };//光源的位置。第4个参数为1，表示点光源；第4个参数量为0，表示平行光束{0.0f, 0.0f, 10.0f, 0.0f}

static GLfloat g_material[] = { 1.0f, 1.0f, 1.0f, 1.0f };//材质
static GLfloat g_rquad = 0;
static GLfloat g_rquad_tmp = 0;

static float g_x_offset = 0.0;
static float g_y_offset = 0.0;
static float g_z_offset = 0.0;
static float g_scale_size = 1;
static int  g_press_x; //鼠标按下时的x坐标
static int  g_press_y; //鼠标按下时的y坐标

//这个n是园的插值数
const int n = 1000;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
int g_view_type = VIEW_FLAT;
int g_draw_content = SHAPE_TRIANGLE;

//辅助计算变化值
Vector3 rotateVal;
Vector3 translateVal;
Vector3 scaleVal = { 1,1,1 };
float scVal = 0;

//辅助计算圆柱和圆锥
Vector3 ver_a;
Vector3 ver_b;
Vector3 ver_c;
Vector3 ver_d;
Vector3 normal;

//用于获取增量时间的静态类
class MyTime
{
public:
	static float totalTime;
	static float deltaTime;
	static float tmpDeltaTime;
};
float MyTime::deltaTime = 0.0f;
float MyTime::totalTime = 0.0f;
float MyTime::tmpDeltaTime = 0.0f;


GLubyte* image;
GLubyte* img;

int tWidth = 64;
int tHeight = 64;

GLuint texName;

void DrawTriangle()
{//绘制三角形
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);                    // 上顶点
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);                    // 左下
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);                    // 右下
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void DrawCube()
{//绘制立方体
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);  //指定面法向
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);   //列举面顶点数据，逆时针顺序
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//前---------------------------- 
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f);

	glVertex3f(-1.0f, 1.0f, -1.0f);

	glVertex3f(1.0f, 1.0f, -1.0f);

	glVertex3f(1.0f, -1.0f, -1.0f);
	//后----------------------------  
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//上----------------------------  
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//下----------------------------  
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//右----------------------------  
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//左----------------------------*/  
	glEnd();
}

void DrawCircle()
{//绘制圆
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < n; ++i) {
		glTexCoord2f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i));
		glVertex3f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i), 0.0f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void DrawCylinder()
{
	//下底面
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	for (int i = 0; i < n; i++)
	{
		glTexCoord2f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i));
		glVertex3f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i), -1.0f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//上底面
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < n; i++)
	{
		glTexCoord2f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i));
		glVertex3f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i), 1.0f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//侧面
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	for (int i = 0; i < n; i++)
	{
		ver_a = { (double)R * cos(2 * Pi / n * i), (double)R * sin(2 * Pi / n * i), -1.0 };
		ver_b = { (double)R * cos(2 * Pi / n * (i + 1)),(double)R * sin(2 * Pi / n * (i + 1)), -1.0 };
		ver_c = { (double)R * cos(2 * Pi / n * (i + 1)), (double)R * sin(2 * Pi / n * (i + 1)), 1.0 };
		ver_d = { (double)R * cos(2 * Pi / n * i), (double)R * sin(2 * Pi / n * i), 1.0 };
		normal = Cross(ver_c - ver_b, ver_a - ver_b).Normalize();
		glNormal3f(normal.fX, normal.fY, normal.fZ);
		glTexCoord2f((float)i / n * R * Pi, 1.0f);
		glVertex3f(ver_a.fX, ver_a.fY, ver_a.fZ);
		glTexCoord2f((float)(i + 1) / n * R * Pi, 1.0f);
		glVertex3f(ver_b.fX, ver_b.fY, ver_b.fZ);
		glTexCoord2f((float)(i + 1) / n * R * Pi, 0.0f);
		glVertex3f(ver_c.fX, ver_c.fY, ver_c.fZ);
		glTexCoord2f((float)i / n * R * Pi, 0.0f);
		glVertex3f(ver_d.fX, ver_d.fY, ver_d.fZ);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void DrawCone()
{
	//底面
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, -1.0f);
	for (int i = 0; i < n; i++)
	{
		glTexCoord2f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i));
		glVertex3f(R * cos(2 * Pi / n * i), R * sin(2 * Pi / n * i), -1.0f);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//侧面
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	ver_c = { 0,0,1 };
	for (int i = 0; i < n; i++)
	{
		ver_a = { (double)R * cos(2 * Pi / n * i), (double)R * sin(2 * Pi / n * i), -1.0 };
		ver_b = { (double)R * cos(2 * Pi / n * (i + 1)), (double)R * sin(2 * Pi / n * (i + 1)), -1.0 };
		normal = Cross(ver_c - ver_b, ver_a - ver_b).Normalize();
		glNormal3f(normal.fX, normal.fY, normal.fZ);
		glTexCoord2f((float)i / n * R * Pi, 1.0f);
		glVertex3f(ver_a.fX, ver_a.fY, ver_a.fZ);
		glTexCoord2f((float)(i + 1) / n * R * Pi, 1.0f);
		glVertex3f(ver_b.fX, ver_b.fY, ver_b.fZ);
		glTexCoord2f((float)(i + 0.5) / n * R * Pi, 0.0f);
		glVertex3f(ver_c.fX, ver_c.fY, ver_c.fZ);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

}

void DrawBall()
{

}

void DrawModel()
{//TODO: 绘制模型
	auto faceData = CObj::getInstance()->getFaceData();
	auto vertexData = CObj::getInstance()->getVertexData();
	Point* pa, * pb, * pc;
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	for (auto it = faceData.begin(); it != faceData.end(); it++)
	{
		glNormal3f(it->normal.fX, it->normal.fY, it->normal.fZ);
		pa = &vertexData[it->pts[0] - 1];
		pb = &vertexData[it->pts[1] - 1];
		pc = &vertexData[it->pts[2] - 1];

		//glNormal3f(pa->normal.fX, pa->normal.fY, pa->normal.fZ);
		glTexCoord2f(pa->text.fX, pa->text.fY);
		glVertex3f(pa->pos.fX, pa->pos.fY, pa->pos.fZ);

		//glNormal3f(pb->normal.fX, pb->normal.fY, pb->normal.fZ);
		glTexCoord2f(pb->text.fX, pb->text.fY);
		glVertex3f(pb->pos.fX, pb->pos.fY, pb->pos.fZ);

		//glNormal3f(pc->normal.fX, pc->normal.fY, pc->normal.fZ);
		glTexCoord2f(pc->text.fX, pc->text.fY);
		glVertex3f(pc->pos.fX, pc->pos.fY, pc->pos.fZ);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//平移
void TranslateControl()
{
	glTranslatef(translateVal.fX * 0.01f, translateVal.fY * -0.01f, 0.0f);
}

//旋转（在平移之后调用）
void RotateControl()
{
	glRotatef(rotateVal.fY, 1, 0, 0);
	glRotatef(rotateVal.fX, 0, 1, 0);
}

//缩放(在旋转后调用)
void ScaleContrl()
{
	scVal = abs(1 + scaleVal.fX * 0.005f);
	glScalef(scVal, scVal, scVal);
}

void myInit()
{
	glClearColor(.1f, .4f, 9.0f, 1.0f);//用白色清屏

	glLightfv(GL_LIGHT0, GL_AMBIENT, g_light0_ambient);//设置场景的环境光
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light0_diffuse);//设置场景的散射光
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_position);//设置场景的位置

	glMaterialfv(GL_FRONT, GL_DIFFUSE, g_material);//指定用于光照计算的当前材质属性

	glEnable(GL_LIGHTING);//开启灯光
	glEnable(GL_LIGHT0);//开启光照0
	glShadeModel(GL_SMOOTH); //设置着色模式为光滑着色
	glEnable(GL_DEPTH_TEST);//启用深度测试

	glMatrixMode(GL_MODELVIEW); //指定当前矩阵为模型视景矩阵
	glLoadIdentity(); //将当前的用户坐标系的原点移到了屏幕中心：类似于一个复位操作
	gluLookAt(0.0, 0.0, 8.0,
		0, 0, 0,
		0, 1, 0);//该函数定义一个视图矩阵，并与当前矩阵相乘.
	//第一组eyex, eyey,eyez 相机在世界坐标的位置;第二组centerx,centery,centerz 相机镜头对准的物体在世界坐标的位置;第三组upx,upy,upz 相机向上的方向在世界坐标中的方向
}

#if !_DEBUG
string TCHAR2STRING(TCHAR* str) {
	std::string strstr;
	try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof(char)];

		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	}
	catch (exception e)
	{
	}

	return strstr;
}
#endif // _DEBUG

void loadObjFile(void)
{//加载模型

	//调用系统对话框

#if _DEBUG 
	OPENFILENAME  fname;
	ZeroMemory(&fname, sizeof(fname));
	char strfile[200] = "";
	char szFilter[] = TEXT("All\0*.*\0OBJ Files(*.OBJ)\0\0");
	fname.lStructSize = sizeof(OPENFILENAME);
	fname.hwndOwner = NULL;
	fname.hInstance = NULL;
	fname.lpstrFilter = szFilter;
	fname.lpstrCustomFilter = NULL;
	fname.nFilterIndex = 0;
	fname.nMaxCustFilter = 0;
	fname.lpstrFile = (LPSTR)strfile;
	fname.nMaxFile = 200;
	fname.lpstrFileTitle = NULL;
	fname.nMaxFileTitle = 0;
	fname.lpstrTitle = "请选择一个模型文件";
	fname.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	fname.nFileOffset = 0;
	fname.nFileExtension = 0;
	fname.lpstrDefExt = 0;
	fname.lCustData = NULL;
	fname.lpfnHook = NULL;
	fname.lpTemplateName = NULL;
	fname.lpstrInitialDir = NULL;
	HDC hDC = wglGetCurrentDC();
	HGLRC hRC = wglGetCurrentContext();
	GetOpenFileName(&fname);
	wglMakeCurrent(hDC, hRC);

	CObj::getInstance()->ReadObjFile(fname.lpstrFile); //读入模型文件
#else 
	OPENFILENAME ofn = { 0 };
	TCHAR strFileName[MAX_PATH] = { 0 };	//用于接收文件名
	ofn.lStructSize = sizeof(OPENFILENAME);	//结构体大小
	ofn.hwndOwner = NULL;					//拥有着窗口句柄
	ofn.lpstrFilter = TEXT("All\0*.*\0OBJ Files(*.OBJ)\0\0");	//设置过滤
	ofn.nFilterIndex = 1;	//过滤器索引
	ofn.lpstrFile = strFileName;	//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFileName);	//缓冲区长度
	ofn.lpstrInitialDir = NULL;			//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择一个模型文件"); //窗口标题
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //文件、目录必须存在，隐藏只读选项
	//打开文件对话框
	if (GetOpenFileName(&ofn)) {
		string filePath = TCHAR2STRING(strFileName);
		CObj::getInstance()->ReadObjFile((char*)filePath.c_str()); //读入模型文件
	}
	else {
		//MessageBox(NULL, TEXT("请选择一文件"), NULL, MB_ICONERROR);
	}

#endif
}

void textInitialization(const char * texturePath)
{
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture img
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	GLubyte* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels==4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void myGlutDisplay() //绘图函数， 操作系统在必要时刻就会对窗体进行重新绘制操作
{
	//缓存当前运行时间
	MyTime::tmpDeltaTime = clock();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除颜色缓冲以及深度缓冲
	glEnable(GL_NORMALIZE); //打开法线向量归一化，确保了法线的长度为1

	glMatrixMode(GL_MODELVIEW);//模型视图矩阵
	glPushMatrix(); //压入当前矩阵堆栈

	if (g_draw_content == SHAPE_MODEL)
	{//绘制模型
		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawModel();
	}
	else if (g_draw_content == SHAPE_TRIANGLE)  //画三角形
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawTriangle();
	}
	else if (g_draw_content == SHAPE_CUBE)  //画立方体
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawCube();
	}
	else if (g_draw_content == SHAPE_CIRCLE) // 画圆
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawCircle();
	}
	else if (g_draw_content == SHAPE_CYLINDER)
	{//TODO: 添加画圆柱的代码
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawCylinder();
	}
	else if (g_draw_content == SHAPE_CONE)
	{//TODO：添加画圆锥的代码
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawCone();
	}
	else if (g_draw_content == SHAPE_BALL)
	{//画球
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//平移
		TranslateControl();

		//旋转
		RotateControl();

		//缩放
		ScaleContrl();

		DrawBall();
	}
	glPopMatrix();
	glutSwapBuffers(); //双缓冲

}

void myGlutReshape(int x, int y) //当改变窗口大小时的回调函数
{
	if (y == 0)
	{
		y = 1;
	}

	g_windows_width = x;
	g_windows_height = y;
	double xy_aspect = (float)x / (float)y;
	GLUI_Master.auto_set_viewport(); //自动设置视口大小

	glMatrixMode(GL_PROJECTION);//当前矩阵为投影矩阵
	glLoadIdentity();
	gluPerspective(60.0, xy_aspect, 0.01, 1000.0);//视景体

	glutPostRedisplay(); //标记当前窗口需要重新绘制
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{//键盘时间回调函数

}

void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) //鼠标的状态为按下
	{
		g_press_x = x;
		g_press_y = y;
		if (button == GLUT_LEFT_BUTTON)
		{//按下鼠标的左键表示对模型进行旋转操作
			g_xform_mode = TRANSFORM_ROTATE;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{//按下鼠标的中键表示对模型进行平移操作
			g_xform_mode = TRANSFORM_TRANSLATE;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{//按下鼠标的右键表示对模型进行缩放操作
			g_xform_mode = TRANSFORM_SCALE;
		}
	}
	else if (state == GLUT_UP)
	{//如果没有按鼠标，则不对模型进行任何操作
		g_xform_mode = TRANSFORM_NONE;
	}
}

void myGlutMotion(int x, int y) //处理当鼠标键摁下时,鼠标拖动的事件
{

	if (g_xform_mode == TRANSFORM_ROTATE) //旋转
	{//TODO:添加鼠标移动控制模型旋转参数的代码
		//增量距离
		rotateVal.fX += x - static_cast<double>(g_press_x);
		rotateVal.fY += y - static_cast<double>(g_press_y);
		g_press_x = x;
		g_press_y = y;
	}
	else if (g_xform_mode == TRANSFORM_SCALE) //缩放
	{//TODO:添加鼠标移动控制模型缩放参数的代码
		scaleVal.fX += x - static_cast<double>(g_press_x);
		g_press_x = x;
	}
	else if (g_xform_mode == TRANSFORM_TRANSLATE) //平移
	{//TODO:添加鼠标移动控制模型平移参数的代码
		translateVal.fX += x - static_cast<double>(g_press_x);
		translateVal.fY += y - static_cast<double>(g_press_y);
		g_press_x = x;
		g_press_y = y;
	}

	// force the redraw function
	glutPostRedisplay();
}

void myGlutIdle(void) //空闲回调函数
{
	if (glutGetWindow() != g_main_window)
		glutSetWindow(g_main_window);

	MyTime::deltaTime = clock() - MyTime::tmpDeltaTime;
	MyTime::totalTime += MyTime::deltaTime;

	g_rquad += 0.03f * MyTime::deltaTime;// 增加旋转变量

	if (g_xform_mode == TRANSFORM_NONE) {
	}

	glutPostRedisplay();
}

void glui_control(int control) //处理控件的返回值
{
	switch (control)
	{
	case CRTL_LOAD://选择“open”控件
		loadObjFile();
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_CHANGE://选择Type面板
		if (g_view_type == VIEW_POINT)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // 设置两面均为顶点绘制方式
		}
		else if (g_view_type == VIEW_WIRE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 设置两面均为线段绘制方式
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 设置两面为填充方式
		}
		break;
	case CRTL_TRIANGLE:
		g_draw_content = SHAPE_TRIANGLE;
		break;
	case CRTL_CUBE:
		g_draw_content = SHAPE_CUBE;
		break;
	case CRTL_CIRCLE:
		g_draw_content = SHAPE_CIRCLE;
		break;
	case CRTL_CYLINDER:
		g_draw_content = SHAPE_CYLINDER;
		break;
	case CRTL_CONE:
		g_draw_content = SHAPE_CONE;
		break;
	case CRTL_MODEL:
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_BALL:
		g_draw_content = SHAPE_BALL;
		break;
	default:
		break;
	}
}

void myGlui()
{
	GLUI_Master.set_glutDisplayFunc(myGlutDisplay); //注册渲染事件回调函数， 系统在需要对窗体进行重新绘制操作时调用
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);  //注册窗口大小改变事件回调函数
	GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);//注册键盘输入事件回调函数
	glutMotionFunc(myGlutMotion);//注册鼠标移动事件回调函数
	GLUI_Master.set_glutMouseFunc(myGlutMouse);//注册鼠标点击事件回调函数
	GLUI_Master.set_glutIdleFunc(myGlutIdle); //为GLUI注册一个标准的GLUT空闲回调函数，当系统处于空闲时,就会调用该注册的函数

	//GLUI
	GLUI* glui = GLUI_Master.create_glui_subwindow(g_main_window, GLUI_SUBWINDOW_RIGHT); //新建子窗体，位于主窗体的右部 
	new GLUI_StaticText(glui, "GLUI"); //在GLUI下新建一个静态文本框，输出内容为“GLUI”
	new GLUI_Separator(glui); //新建分隔符
	new GLUI_Button(glui, "Open", CRTL_LOAD, glui_control); //新建按钮控件，参数分别为：所属窗体、名字、ID、回调函数，当按钮被触发时,它会被调用.
	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);//新建退出按钮，当按钮被触发时,退出程序

	GLUI_Panel* type_panel = glui->add_panel("Type"); //在子窗体glui中新建面板，名字为“Type”
	GLUI_RadioGroup* radio = glui->add_radiogroup_to_panel(type_panel, &g_view_type, CRTL_CHANGE, glui_control); //在Type面板中添加一组单选按钮
	glui->add_radiobutton_to_group(radio, "points");
	glui->add_radiobutton_to_group(radio, "wire");
	glui->add_radiobutton_to_group(radio, "flat");

	GLUI_Panel* draw_panel = glui->add_panel("Draw"); //在子窗体glui中新建面板，名字为“Draw”
	new GLUI_Button(draw_panel, "Triangle", CRTL_TRIANGLE, glui_control);
	new GLUI_Button(draw_panel, "Cube", CRTL_CUBE, glui_control);
	new GLUI_Button(draw_panel, "Circle", CRTL_CIRCLE, glui_control);
	new GLUI_Button(draw_panel, "Cylinder", CRTL_CYLINDER, glui_control);
	new GLUI_Button(draw_panel, "Cone", CRTL_CONE, glui_control);
	new GLUI_Button(draw_panel, "Ball", CRTL_BALL, glui_control);
	new GLUI_Button(draw_panel, "Model", CRTL_MODEL, glui_control);

	glui->set_main_gfx_window(g_main_window); //将子窗体glui与主窗体main_window绑定，当窗体glui中的控件的值发生过改变，则该glui窗口被重绘
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
}

int main(int argc, char* argv[]) //程序入口
{
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/

	//freopen("log.txt", "w", stdout);//重定位，将输出放入log.txt文件中
	glutInit(&argc, argv);//初始化glut
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//初始化渲染模式
	glutInitWindowPosition(200, 200); //初始化窗口位置
	glutInitWindowSize(800, 600); //初始化窗口大小

	g_main_window = glutCreateWindow("Model Viewer"); //创建主窗体Model Viewer

	myGlui();
	myInit();
	textInitialization("Model/blender/castle_out.png");

	glutMainLoop();//进入glut消息循环

	free(image);

	return EXIT_SUCCESS;
}

