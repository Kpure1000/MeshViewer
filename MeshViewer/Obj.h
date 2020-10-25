#ifndef OBJ_CLASS
#define OBJ_CLASS

#include <vector>
#include <map>
#include <cmath>
#include<string>
#include<gl/glut.h>
#include<iostream>
using namespace std;


void MakeMap(int w, int h, GLubyte* image);
GLubyte* ReadImage(int& w, int& h, string objName);

struct Vector3;
Vector3 operator + (const Vector3& one, const Vector3& two);
Vector3 operator - (const Vector3& one, const Vector3& two);
Vector3 operator * (const Vector3& one, double scale);
Vector3 operator / (const Vector3& one, double scale);
Vector3 Cross(Vector3& one, Vector3& two);
Vector3 Dot(Vector3& va, Vector3& vb);

struct Vector3
{
	double fX;
	double fY;
	double fZ;
	Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : fX(x), fY(y), fZ(z) {}
	Vector3 operator +=(const Vector3& v) { return *this = *this + v; }
	Vector3 operator -=(const Vector3& v) { return *this = *this - v; }
	Vector3 operator *=(double const& scale) { return *this = *this * scale; }
	Vector3 operator /=(double const& scale) { return *this = *this / scale; }
	double Magnitude() { return sqrt(fX * fX + fY * fY + fZ * fZ); }
	Vector3 Normalize()//归一化
	{
		double fLen = Magnitude();
		if (fabs(fLen) > 1e-6)
		{
			fX /= fLen;
			fY /= fLen;
			fZ /= fLen;
		}
		return *this;
	}
};

struct Point
{
	Point() {}
	Point(Vector3 position) :pos(position)
	{
		normal = pos;
	}
	Point(Vector3 position, Vector3 texCoords) :pos(position), text(texCoords)
	{
		normal = pos;
	}
	Vector3 pos;
	Vector3 normal; // 顶点法线
	Vector3 text; //  纹理坐标
};

struct Face
{
	Face()
	{
		pts[0] = 0;
		pts[1] = 0;
		pts[2] = 0;
	}
	Face(int a, int b, int c)
	{
		pts[0] = a;
		pts[1] = b;
		pts[2] = c;
	}
	int pts[3];
	Vector3 normal;
};

class CObj
{
public:
	~CObj(void);

	static CObj* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new CObj();
		}
		return instance;
	}

	bool ReadObjFile(char* pcszFileName);//读入模型文件

	vector<Point> getVertexData();
	vector<Face> getFaceData();

	vector<Point>* m_pts;
	vector<Face>* m_faces;
private:
	static CObj* instance;

	CObj(void);


	//存在字典里面，减少重复读文件加载次数
	std::map <string, vector<Point>> vertexDataMap;
	std::map <string, vector<Face>> faceDataMap;

	std::vector<Point>& findVertexContainer(char* fileName);
	std::vector<Face>& findFaceContainer(char* fileName);
	void UnifyModel();//单位化模型
	void ComputeFaceNormal(Face& f);//计算面的法线
};

#endif

