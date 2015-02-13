#include "PrimitiveGenerator.h"






void Candy::PrimitiveGeometry::cylinder(IndexedMesh<glm::vec3, unsigned>& m, float radius, float height, int div)
{
	//vertices top
	m.vertices.push_back(vec3(0,height/2,0));
	for(float i=0;i<div;i++)
	{
		m.vertices.push_back(vec3(radius*cos(i*6.283/div),height/2,radius*sin(i*6.283/div)));
	}

	//indizes top
	for(int i=0;i<div;i++)
	{
		m.indices.push_back(0);
		m.indices.push_back((i)%div+1);
		m.indices.push_back((i+1)%div+1);
	}

	int off = div+1;
	//vertices bottom
	m.vertices.push_back(vec3(0,-height/2,0));
	for(int i=0;i<div;i++)
	{
		m.vertices.push_back(vec3(radius*cos(i*6.283/div),-height/2,radius*sin(i*6.283/div)));
	}

	//indizes bottom
	for(int i=0;i<div;i++)
	{
		m.indices.push_back(off);
		m.indices.push_back((i+1)%div+off+1);
		m.indices.push_back((i)%div+off+1);
	}

	//indizes side
	for(int i=0;i<div;i++)
	{
		m.indices.push_back((i+0)%div+1);
		m.indices.push_back((i+0)%div+1+off);
		m.indices.push_back((i+1)%div+1);


		m.indices.push_back((i+0)%div+1+off);
		m.indices.push_back((i+1)%div+1+off);
		m.indices.push_back((i+1)%div+1);
	}
}
