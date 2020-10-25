#pragma once
#include"Obj.h"
using namespace std;

struct Vertex
{
	Vertex() {}
	Vertex(Vector3 position) :pos(position)
	{

	}
	Vector3 pos;
	Vector3 normal;
	Vector3 texCoords;
};

struct Surface
{
	Surface() {}
	int vts[3];
	Vector3 normal;
};

class Mesh
{
public:


private:

};

