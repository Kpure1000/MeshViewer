#include "Obj.h"
#include "stb_image.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <string>
#include<gl/glut.h>

Vector3 operator + (const Vector3& one, const Vector3& two) //两个向量相加
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //两个向量相减
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //向量与数的乘操作
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //向量与数的除操作
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two)
{//计算两个向量的叉积
	Vector3 vCross;

	vCross.fX = ((one.fY * two.fZ) - (one.fZ * two.fY));
	vCross.fY = ((one.fZ * two.fX) - (one.fX * two.fZ));
	vCross.fZ = ((one.fX * two.fY) - (one.fY * two.fX));

	return vCross;
}

Vector3 Dot(Vector3& va, Vector3& vb)
{
	return va.fX * vb.fX + va.fY * vb.fY + va.fZ * vb.fZ;
}

CObj* CObj::instance = nullptr;

CObj::CObj(void)
{
	m_pts = new vector<Point>();
	m_faces = new vector<Face>();
}

CObj::~CObj(void)
{
}

bool CObj::ReadObjFile(char* pcszFileName)
{//读取模型文件
	printf("模型文件名称: %s\n", pcszFileName);
	
	m_pts = &findVertexContainer(pcszFileName);
	m_faces = &findFaceContainer(pcszFileName);
	GLubyte* img;
	int w, h;
	img = ReadImage(w, h, "wall.jpg");

	int vt_count = 0;

	//如果是第一次读这个文件
	if (m_pts->empty() && m_faces->empty())
	{
		printf("缓存为空，读取文件\n");
		std::ifstream reader(pcszFileName);
		//TODO：将模型文件中的点和面数据分别存入m_pts和m_faces中
		if (reader)
		{
			char buffer[255];
			float u, v;
			float x, y, z;
			int a, b, c;
			int other;
			while (!reader.getline(buffer, 255).eof())
			{
				if (buffer[0] == 'v')//顶点
				{
					if (buffer[1] == 't')
					{
						sscanf_s(buffer, "vt %f %f", &u, &v);
						if (m_pts->size() > 0)
						{
							if (vt_count < m_pts->size())
								(*m_pts)[vt_count].text = { u,v,0 };
							vt_count++;
						}
					}
					else
					{
						sscanf_s(buffer, "v %f %f %f", &x, &y, &z);
						m_pts->push_back(Point({ x,y,z }));
					}
				}
				else if (buffer[0] == 'f')//面
				{
					sscanf_s(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a, &other, &other, &b, &other, &other, &c, &other, &other);
					m_faces->push_back(Face(a, b, c));
				}
			}
			reader.close();
			printf("读取文件完毕\n");
		}
		else {
			return false;
		}
		//求所有的法线
		Point* v_a, * v_b, * v_c;
		for (auto it = m_faces->begin(); it != m_faces->end(); it++)
		{
			v_a = &(*m_pts)[it->pts[0] - 1];
			v_b = &(*m_pts)[it->pts[1] - 1];
			v_c = &(*m_pts)[it->pts[2] - 1];
			//首先计算面元法线
			it->normal = Cross(v_c->pos - v_b->pos, v_a->pos - v_b->pos).Normalize();
			//然后计算顶点法线
			v_a->normal += it->normal;
			v_b->normal += it->normal;
			v_c->normal += it->normal;
		}

		//顶点法线归一化
		for (auto it = m_pts->begin(); it != m_pts->end(); it++)
		{
			it->normal = it->normal.Normalize();
		}

		UnifyModel(); //将模型归一化
	}

	printf("该模型 顶点数: %d, 贴图坐标数: %d, 面数: %d\n", m_pts->size(),vt_count, m_faces->size());

	return true;
}

vector<Point> CObj::getVertexData()
{
	return (*m_pts);
}

vector<Face> CObj::getFaceData()
{
	return (*m_faces);
}

std::vector<Point>& CObj::findVertexContainer(char* fileName)
{
	string fstr(fileName);

	if (vertexDataMap.find(fstr) == vertexDataMap.end())
	{
		vertexDataMap[fstr] = vector<Point>();
		printf("该模型的顶点数据未在缓存池中，现创建key\n");
	}
	return vertexDataMap[fstr];
}

std::vector<Face>& CObj::findFaceContainer(char* fileName)
{
	string fstr(fileName);
	if (faceDataMap.find(fstr) == faceDataMap.end())
	{
		faceDataMap[fstr] = vector<Face>();
		printf("该模型的面数据未在缓存池中，现创建key\n");
	}
	return faceDataMap[fstr];
}
void CObj::UnifyModel()
{//为统一显示不同尺寸的模型，将模型归一化，将模型尺寸缩放到0.0-1.0之间
//原理：找出模型的边界最大和最小值，进而找出模型的中心
//以模型的中心点为基准对模型顶点进行缩放
//TODO:添加模型归一化代码
	Vector3 minAxis;
	Vector3 maxAxis;

	for (auto it = m_pts->begin(); it != m_pts->end(); it++)
	{
		minAxis.fX = it->pos.fX < minAxis.fX ? it->pos.fX : minAxis.fX;
		minAxis.fY = it->pos.fY < minAxis.fY ? it->pos.fY : minAxis.fY;
		minAxis.fZ = it->pos.fZ < minAxis.fZ ? it->pos.fZ : minAxis.fZ;
		maxAxis.fX = it->pos.fX > maxAxis.fX ? it->pos.fX : maxAxis.fX;
		maxAxis.fY = it->pos.fY > maxAxis.fY ? it->pos.fY : maxAxis.fY;
		maxAxis.fZ = it->pos.fZ > maxAxis.fZ ? it->pos.fZ : maxAxis.fZ;
	}
	Vector3 center = (minAxis + maxAxis) / 2;
	float maxVal = (maxAxis - minAxis).Magnitude();
	for (auto it = m_pts->begin(); it != m_pts->end(); it++)
	{
		it->pos -= center;
		it->pos /= maxVal;
	}
}

void CObj::ComputeFaceNormal(Face& f)
{//TODO:计算面f的法向量，并保存
	//弃用
}

//在这里画棋盘图像
void MakeMap(int w, int h, GLubyte* image)
{
	image = (GLubyte*)malloc(sizeof(GLubyte) * w * h * 4);
	GLubyte c;
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			//计算纹理颜色
			image[i * h * 4 + j * 4] = GLubyte(c * 0.2);
			image[i * h * 4 + j * 4 + 1] = GLubyte(c * 0.9);
			image[i * h * 4 + j * 4 + 2] = GLubyte(c * 0.3);
			image[i * h * 4 + j * 4 + 3] = GLubyte(255);

		}
	}
}

GLubyte* ReadImage(int& w, int& h, string objName)
{
	int channel;
	stbi_set_flip_vertically_on_load(true);
	auto image = (GLubyte*)stbi_load(objName.c_str(), &w, &h, &channel, 0);
	if (image != nullptr)
	{
		cout << "读取图片成功" << endl;
		return image;
	}
	return nullptr;
}