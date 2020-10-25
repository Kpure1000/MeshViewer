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
static GLfloat g_light0_ambient[] = { 0.3f, 0.6f, 0.2f, 0.5f };//������
static GLfloat g_light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 0.5f };//ɢ���
static GLfloat g_light0_specular[] = { 0.0f,0.0f, 0.0f, 0.5f }; //�����
static GLfloat g_light0_position[] = { 0.0f, 0.0f, 30.0f, 0.5f };//��Դ��λ�á���4������Ϊ1����ʾ���Դ����4��������Ϊ0����ʾƽ�й���{0.0f, 0.0f, 10.0f, 0.0f}

static GLfloat g_material[] = { 1.0f, 1.0f, 1.0f, 1.0f };//����
static GLfloat g_rquad = 0;
static GLfloat g_rquad_tmp = 0;

static float g_x_offset = 0.0;
static float g_y_offset = 0.0;
static float g_z_offset = 0.0;
static float g_scale_size = 1;
static int  g_press_x; //��갴��ʱ��x����
static int  g_press_y; //��갴��ʱ��y����

//���n��԰�Ĳ�ֵ��
const int n = 1000;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;
int g_view_type = VIEW_FLAT;
int g_draw_content = SHAPE_TRIANGLE;

//��������仯ֵ
Vector3 rotateVal;
Vector3 translateVal;
Vector3 scaleVal = { 1,1,1 };
float scVal = 0;

//��������Բ����Բ׶
Vector3 ver_a;
Vector3 ver_b;
Vector3 ver_c;
Vector3 ver_d;
Vector3 normal;

//���ڻ�ȡ����ʱ��ľ�̬��
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
{//����������
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	glTexCoord2f(0.5f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);                    // �϶���
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);                    // ����
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);                    // ����
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void DrawCube()
{//����������
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);  //ָ���淨��
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);   //�о��涥�����ݣ���ʱ��˳��
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//ǰ---------------------------- 
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1.0f, -1.0f, -1.0f);

	glVertex3f(-1.0f, 1.0f, -1.0f);

	glVertex3f(1.0f, 1.0f, -1.0f);

	glVertex3f(1.0f, -1.0f, -1.0f);
	//��----------------------------  
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	//��----------------------------  
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	//��----------------------------  
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	//��----------------------------  
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	//��----------------------------*/  
	glEnd();
}

void DrawCircle()
{//����Բ
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
	//�µ���
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

	//�ϵ���
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

	//����
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
	//����
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

	//����
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
{//TODO: ����ģ��
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

//ƽ��
void TranslateControl()
{
	glTranslatef(translateVal.fX * 0.01f, translateVal.fY * -0.01f, 0.0f);
}

//��ת����ƽ��֮����ã�
void RotateControl()
{
	glRotatef(rotateVal.fY, 1, 0, 0);
	glRotatef(rotateVal.fX, 0, 1, 0);
}

//����(����ת�����)
void ScaleContrl()
{
	scVal = abs(1 + scaleVal.fX * 0.005f);
	glScalef(scVal, scVal, scVal);
}

void myInit()
{
	glClearColor(.1f, .4f, 9.0f, 1.0f);//�ð�ɫ����

	glLightfv(GL_LIGHT0, GL_AMBIENT, g_light0_ambient);//���ó����Ļ�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light0_diffuse);//���ó�����ɢ���
	glLightfv(GL_LIGHT0, GL_POSITION, g_light0_position);//���ó�����λ��

	glMaterialfv(GL_FRONT, GL_DIFFUSE, g_material);//ָ�����ڹ��ռ���ĵ�ǰ��������

	glEnable(GL_LIGHTING);//�����ƹ�
	glEnable(GL_LIGHT0);//��������0
	glShadeModel(GL_SMOOTH); //������ɫģʽΪ�⻬��ɫ
	glEnable(GL_DEPTH_TEST);//������Ȳ���

	glMatrixMode(GL_MODELVIEW); //ָ����ǰ����Ϊģ���Ӿ�����
	glLoadIdentity(); //����ǰ���û�����ϵ��ԭ���Ƶ�����Ļ���ģ�������һ����λ����
	gluLookAt(0.0, 0.0, 8.0,
		0, 0, 0,
		0, 1, 0);//�ú�������һ����ͼ���󣬲��뵱ǰ�������.
	//��һ��eyex, eyey,eyez ��������������λ��;�ڶ���centerx,centery,centerz �����ͷ��׼�����������������λ��;������upx,upy,upz ������ϵķ��������������еķ���
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
{//����ģ��

	//����ϵͳ�Ի���

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
	fname.lpstrTitle = "��ѡ��һ��ģ���ļ�";
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

	CObj::getInstance()->ReadObjFile(fname.lpstrFile); //����ģ���ļ�
#else 
	OPENFILENAME ofn = { 0 };
	TCHAR strFileName[MAX_PATH] = { 0 };	//���ڽ����ļ���
	ofn.lStructSize = sizeof(OPENFILENAME);	//�ṹ���С
	ofn.hwndOwner = NULL;					//ӵ���Ŵ��ھ��
	ofn.lpstrFilter = TEXT("All\0*.*\0OBJ Files(*.OBJ)\0\0");	//���ù���
	ofn.nFilterIndex = 1;	//����������
	ofn.lpstrFile = strFileName;	//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFileName);	//����������
	ofn.lpstrInitialDir = NULL;			//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ��һ��ģ���ļ�"); //���ڱ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
	//���ļ��Ի���
	if (GetOpenFileName(&ofn)) {
		string filePath = TCHAR2STRING(strFileName);
		CObj::getInstance()->ReadObjFile((char*)filePath.c_str()); //����ģ���ļ�
	}
	else {
		//MessageBox(NULL, TEXT("��ѡ��һ�ļ�"), NULL, MB_ICONERROR);
	}

#endif
}

void textInitialization(const char * texturePath)
{
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
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

void myGlutDisplay() //��ͼ������ ����ϵͳ�ڱ�Ҫʱ�̾ͻ�Դ���������»��Ʋ���
{
	//���浱ǰ����ʱ��
	MyTime::tmpDeltaTime = clock();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����ɫ�����Լ���Ȼ���
	glEnable(GL_NORMALIZE); //�򿪷���������һ����ȷ���˷��ߵĳ���Ϊ1

	glMatrixMode(GL_MODELVIEW);//ģ����ͼ����
	glPushMatrix(); //ѹ�뵱ǰ�����ջ

	if (g_draw_content == SHAPE_MODEL)
	{//����ģ��
		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawModel();
	}
	else if (g_draw_content == SHAPE_TRIANGLE)  //��������
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawTriangle();
	}
	else if (g_draw_content == SHAPE_CUBE)  //��������
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawCube();
	}
	else if (g_draw_content == SHAPE_CIRCLE) // ��Բ
	{
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawCircle();
	}
	else if (g_draw_content == SHAPE_CYLINDER)
	{//TODO: ��ӻ�Բ���Ĵ���
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawCylinder();
	}
	else if (g_draw_content == SHAPE_CONE)
	{//TODO����ӻ�Բ׶�Ĵ���
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawCone();
	}
	else if (g_draw_content == SHAPE_BALL)
	{//����
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -6.0f);

		//ƽ��
		TranslateControl();

		//��ת
		RotateControl();

		//����
		ScaleContrl();

		DrawBall();
	}
	glPopMatrix();
	glutSwapBuffers(); //˫����

}

void myGlutReshape(int x, int y) //���ı䴰�ڴ�Сʱ�Ļص�����
{
	if (y == 0)
	{
		y = 1;
	}

	g_windows_width = x;
	g_windows_height = y;
	double xy_aspect = (float)x / (float)y;
	GLUI_Master.auto_set_viewport(); //�Զ������ӿڴ�С

	glMatrixMode(GL_PROJECTION);//��ǰ����ΪͶӰ����
	glLoadIdentity();
	gluPerspective(60.0, xy_aspect, 0.01, 1000.0);//�Ӿ���

	glutPostRedisplay(); //��ǵ�ǰ������Ҫ���»���
}

void myGlutKeyboard(unsigned char Key, int x, int y)
{//����ʱ��ص�����

}

void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) //����״̬Ϊ����
	{
		g_press_x = x;
		g_press_y = y;
		if (button == GLUT_LEFT_BUTTON)
		{//�������������ʾ��ģ�ͽ�����ת����
			g_xform_mode = TRANSFORM_ROTATE;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{//���������м���ʾ��ģ�ͽ���ƽ�Ʋ���
			g_xform_mode = TRANSFORM_TRANSLATE;
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{//���������Ҽ���ʾ��ģ�ͽ������Ų���
			g_xform_mode = TRANSFORM_SCALE;
		}
	}
	else if (state == GLUT_UP)
	{//���û�а���꣬�򲻶�ģ�ͽ����κβ���
		g_xform_mode = TRANSFORM_NONE;
	}
}

void myGlutMotion(int x, int y) //������������ʱ,����϶����¼�
{

	if (g_xform_mode == TRANSFORM_ROTATE) //��ת
	{//TODO:�������ƶ�����ģ����ת�����Ĵ���
		//��������
		rotateVal.fX += x - static_cast<double>(g_press_x);
		rotateVal.fY += y - static_cast<double>(g_press_y);
		g_press_x = x;
		g_press_y = y;
	}
	else if (g_xform_mode == TRANSFORM_SCALE) //����
	{//TODO:�������ƶ�����ģ�����Ų����Ĵ���
		scaleVal.fX += x - static_cast<double>(g_press_x);
		g_press_x = x;
	}
	else if (g_xform_mode == TRANSFORM_TRANSLATE) //ƽ��
	{//TODO:�������ƶ�����ģ��ƽ�Ʋ����Ĵ���
		translateVal.fX += x - static_cast<double>(g_press_x);
		translateVal.fY += y - static_cast<double>(g_press_y);
		g_press_x = x;
		g_press_y = y;
	}

	// force the redraw function
	glutPostRedisplay();
}

void myGlutIdle(void) //���лص�����
{
	if (glutGetWindow() != g_main_window)
		glutSetWindow(g_main_window);

	MyTime::deltaTime = clock() - MyTime::tmpDeltaTime;
	MyTime::totalTime += MyTime::deltaTime;

	g_rquad += 0.03f * MyTime::deltaTime;// ������ת����

	if (g_xform_mode == TRANSFORM_NONE) {
	}

	glutPostRedisplay();
}

void glui_control(int control) //����ؼ��ķ���ֵ
{
	switch (control)
	{
	case CRTL_LOAD://ѡ��open���ؼ�
		loadObjFile();
		g_draw_content = SHAPE_MODEL;
		break;
	case CRTL_CHANGE://ѡ��Type���
		if (g_view_type == VIEW_POINT)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // ���������Ϊ������Ʒ�ʽ
		}
		else if (g_view_type == VIEW_WIRE)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // ���������Ϊ�߶λ��Ʒ�ʽ
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // ��������Ϊ��䷽ʽ
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
	GLUI_Master.set_glutDisplayFunc(myGlutDisplay); //ע����Ⱦ�¼��ص������� ϵͳ����Ҫ�Դ���������»��Ʋ���ʱ����
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);  //ע�ᴰ�ڴ�С�ı��¼��ص�����
	GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);//ע����������¼��ص�����
	glutMotionFunc(myGlutMotion);//ע������ƶ��¼��ص�����
	GLUI_Master.set_glutMouseFunc(myGlutMouse);//ע��������¼��ص�����
	GLUI_Master.set_glutIdleFunc(myGlutIdle); //ΪGLUIע��һ����׼��GLUT���лص���������ϵͳ���ڿ���ʱ,�ͻ���ø�ע��ĺ���

	//GLUI
	GLUI* glui = GLUI_Master.create_glui_subwindow(g_main_window, GLUI_SUBWINDOW_RIGHT); //�½��Ӵ��壬λ����������Ҳ� 
	new GLUI_StaticText(glui, "GLUI"); //��GLUI���½�һ����̬�ı����������Ϊ��GLUI��
	new GLUI_Separator(glui); //�½��ָ���
	new GLUI_Button(glui, "Open", CRTL_LOAD, glui_control); //�½���ť�ؼ��������ֱ�Ϊ���������塢���֡�ID���ص�����������ť������ʱ,���ᱻ����.
	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);//�½��˳���ť������ť������ʱ,�˳�����

	GLUI_Panel* type_panel = glui->add_panel("Type"); //���Ӵ���glui���½���壬����Ϊ��Type��
	GLUI_RadioGroup* radio = glui->add_radiogroup_to_panel(type_panel, &g_view_type, CRTL_CHANGE, glui_control); //��Type��������һ�鵥ѡ��ť
	glui->add_radiobutton_to_group(radio, "points");
	glui->add_radiobutton_to_group(radio, "wire");
	glui->add_radiobutton_to_group(radio, "flat");

	GLUI_Panel* draw_panel = glui->add_panel("Draw"); //���Ӵ���glui���½���壬����Ϊ��Draw��
	new GLUI_Button(draw_panel, "Triangle", CRTL_TRIANGLE, glui_control);
	new GLUI_Button(draw_panel, "Cube", CRTL_CUBE, glui_control);
	new GLUI_Button(draw_panel, "Circle", CRTL_CIRCLE, glui_control);
	new GLUI_Button(draw_panel, "Cylinder", CRTL_CYLINDER, glui_control);
	new GLUI_Button(draw_panel, "Cone", CRTL_CONE, glui_control);
	new GLUI_Button(draw_panel, "Ball", CRTL_BALL, glui_control);
	new GLUI_Button(draw_panel, "Model", CRTL_MODEL, glui_control);

	glui->set_main_gfx_window(g_main_window); //���Ӵ���glui��������main_window�󶨣�������glui�еĿؼ���ֵ�������ı䣬���glui���ڱ��ػ�
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
}

int main(int argc, char* argv[]) //�������
{
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/

	//freopen("log.txt", "w", stdout);//�ض�λ�����������log.txt�ļ���
	glutInit(&argc, argv);//��ʼ��glut
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);//��ʼ����Ⱦģʽ
	glutInitWindowPosition(200, 200); //��ʼ������λ��
	glutInitWindowSize(800, 600); //��ʼ�����ڴ�С

	g_main_window = glutCreateWindow("Model Viewer"); //����������Model Viewer

	myGlui();
	myInit();
	textInitialization("Model/blender/castle_out.png");

	glutMainLoop();//����glut��Ϣѭ��

	free(image);

	return EXIT_SUCCESS;
}

