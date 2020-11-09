#include "Obj.h"
#include <stb/stb_image_write.h>
#include<stb/stb_image.h>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <string>
#include<gl/glut.h>

Vector3 operator + (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX + two.fX, one.fY + two.fY, one.fZ + two.fZ);
}

Vector3 operator - (const Vector3& one, const Vector3& two) //�����������
{
	return Vector3(one.fX - two.fX, one.fY - two.fY, one.fZ - two.fZ);
}

Vector3 operator * (const Vector3& one, double scale) //���������ĳ˲���
{
	return Vector3(one.fX * scale, one.fY * scale, one.fZ * scale);
}

Vector3 operator / (const Vector3& one, double scale) //���������ĳ�����
{
	return one * (1.0 / scale);
}

Vector3 Cross(Vector3& one, Vector3& two)
{//�������������Ĳ��
	Vector3 vCross;

	vCross.fX = ((one.fY * two.fZ) - (one.fZ * two.fY));
	vCross.fY = ((one.fZ * two.fX) - (one.fX * two.fZ));
	vCross.fZ = ((one.fX * two.fY) - (one.fY * two.fX));

	return vCross;
}

float Dot(Vector3& va, Vector3& vb)
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
{//��ȡģ���ļ�
	printf("ģ���ļ�����: %s\n", pcszFileName);
	
	m_pts = &findVertexContainer(pcszFileName);
	m_faces = &findFaceContainer(pcszFileName);
	m_tex = &findTexContainer(pcszFileName);
	m_nor = &findNorContainer(pcszFileName);

	int vt_count = 0;

	//����ǵ�һ�ζ�����ļ�
	if (m_pts->empty() && m_faces->empty() && m_tex->empty())
	{
		printf("����Ϊ�գ���ȡ�ļ�\n");
		std::ifstream reader(pcszFileName);
		//TODO����ģ���ļ��еĵ�������ݷֱ����m_pts��m_faces��
		if (reader)
		{
			char buffer[255];
			float nx, ny, nz;
			float u, v;
			float x, y, z;
			int a, b, c;
			int vta, vtb, vtc;
			int vna, vnb, vnc;
			while (!reader.getline(buffer, 255).eof())
			{
				if (buffer[0] == 'v')//����
				{
					if (buffer[1] == 't')
					{
						sscanf_s(buffer, "vt %f %f", &u, &v);
						m_tex->push_back({ u,v,0 });
						//cout << "UV����: " << u << ", " << v << endl;
					}
					else if (buffer[1] == 'n')
					{
						sscanf_s(buffer, "vn %f %f %f", &nx, &ny, &nz);
						m_nor->push_back({ nx,ny,nz });
						//cout << "������: " << nx << ", " << ny << ", " << nz << endl;
					}
					else
					{
						sscanf_s(buffer, "v %f %f %f", &x, &y, &z);
						m_pts->push_back(Point({ x,y,z }));
						//cout << "����: " << x << ", " << y << ", " << z << endl;
					}
				}
				else if (buffer[0] == 'f')//��
				{
					sscanf_s(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a, &vta, &vna, &b, &vtb, &vnb, &c, &vtc, &vnc);
					m_faces->push_back(Face(a, b, c, vta, vtb, vtc, vna, vnb, vnc));
				}
			}
			reader.close();
			printf("��ȡģ���ļ����\n");
		}
		else {
			return false;
		}
		//�����еķ���
		Point* v_a, * v_b, * v_c;
		for (auto it = m_faces->begin(); it != m_faces->end(); it++)
		{
			v_a = &(*m_pts)[it->pts[0] - 1];
			v_b = &(*m_pts)[it->pts[1] - 1];
			v_c = &(*m_pts)[it->pts[2] - 1];
			//���ȼ�����Ԫ����
			it->normal = Cross(v_c->pos - v_b->pos, v_a->pos - v_b->pos).Normalize();
			//Ȼ����㶥�㷨��
			v_a->normal += it->normal;
			v_b->normal += it->normal;
			v_c->normal += it->normal;
		}

		//���㷨�߹�һ��
		for (auto it = m_pts->begin(); it != m_pts->end(); it++)
		{
			it->normal = it->normal.Normalize();
		}

		UnifyModel(); //��ģ�͹�һ��
	}

	printf("��ģ�� ������: %d, ��������: %d, ��ͼ������: %d, ����: %d\n", m_pts->size(), m_nor->size(), m_tex->size(), m_faces->size());


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

vector<Vector3> CObj::getTexCoordData()
{
	return (*m_tex);
}

vector<Vector3> CObj::getNormalData()
{
	return (*m_nor);
}

std::vector<Point>& CObj::findVertexContainer(char* fileName)
{
	string fstr(fileName);

	if (vertexDataMap.find(fstr) == vertexDataMap.end())
	{
		vertexDataMap[fstr] = vector<Point>();
		printf("��ģ�͵Ķ�������δ�ڻ�����У��ִ���key\n");
	}
	return vertexDataMap[fstr];
}

std::vector<Face>& CObj::findFaceContainer(char* fileName)
{
	string fstr(fileName);
	if (faceDataMap.find(fstr) == faceDataMap.end())
	{
		faceDataMap[fstr] = vector<Face>();
		printf("��ģ�͵�������δ�ڻ�����У��ִ���key\n");
	}
	return faceDataMap[fstr];
}
std::vector<Vector3>& CObj::findTexContainer(char* fileName)
{
	string fstr(fileName);
	if (texDataMap.find(fstr) == texDataMap.end())
	{
		texDataMap[fstr] = vector<Vector3>();
		printf("��ģ�͵���ͼ����δ�ڻ�����У��ִ���key\n");
	}
	return texDataMap[fstr];
}
std::vector<Vector3>& CObj::findNorContainer(char* fileName)
{
	string fstr(fileName);
	if (norDataMap.find(fstr) == norDataMap.end())
	{
		norDataMap[fstr] = vector<Vector3>();
		printf("��ģ�͵ķ���������δ�ڻ�����У��ִ���key\n");
	}
	return norDataMap[fstr];
}
void CObj::UnifyModel()
{//Ϊͳһ��ʾ��ͬ�ߴ��ģ�ͣ���ģ�͹�һ������ģ�ͳߴ����ŵ�0.0-1.0֮��
//ԭ���ҳ�ģ�͵ı߽�������Сֵ�������ҳ�ģ�͵�����
//��ģ�͵����ĵ�Ϊ��׼��ģ�Ͷ����������
//TODO:���ģ�͹�һ������
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
{//TODO:������f�ķ�������������
	//����
}

//�����ﻭ����ͼ��
GLubyte* MakeMap(int w, int h, int chennel)
{
	GLubyte* image = (GLubyte*)malloc(sizeof(GLubyte) * w * h * chennel);
	GLubyte c;
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			c = (((i & 0x1) == 0) ^ ((j & 0x1) == 0)) * 255;
			//����������ɫ
			image[i * h * chennel + j * chennel] = GLubyte(c * 0.2);
			image[i * h * chennel + j * chennel + 1] = GLubyte(c * 0.9);
			image[i * h * chennel + j * chennel + 2] = GLubyte(c * 0.3);
			if (chennel >= 4)
				image[i * h * chennel + j * chennel + 3] = GLubyte(255);

		}
	}
	/*stbi_flip_vertically_on_write(true);
	stbi_write_bmp("outImg.bmp", w, h, chennel, image);
	cout << "�������ļ�: outImg" << endl;*/

	return image;
}

GLubyte* ReadImage(int& w, int& h, string objName)
{
	int channel;
	stbi_set_flip_vertically_on_load(true);
	auto image = (GLubyte*)stbi_load(objName.c_str(), &w, &h, &channel, 0);
	if (image != nullptr)
	{
		cout << "��ȡͼƬ�ɹ�" << endl;
		return image;
	}
	return nullptr;
}