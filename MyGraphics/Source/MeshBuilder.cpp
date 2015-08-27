/******************************************************************************/
/*!
\file	MeshBuilder.cpp
\author Ricsson Toh
\brief 
To generate meshes of different shapes
*/
/******************************************************************************/
#include "vertex.h"
#include "MeshBuilder.h"
#include "MyMath.h"

#include <GL\glew.h>
#include <vector>

#define PI 3.14159265

using std::cout;
using std::endl;

/******************************************************************************/
/*!
\brief
Generate the vertices of a reference Axes; Use red for x-axis, green for y-axis, blue for z-axis
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh

\return Pointer to mesh storing VBO/IBO of reference axes
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateAxes(const std::string &meshName)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	
	//const GLfloat vertex_buffer_data[] = {
	v.pos.Set(-1000, 0, 0);
	v.color.Set (1,0,0);
	vertex_buffer_data.push_back(v);	

	v.pos.Set(1000, 0, 0);
	v.color.Set (1,0,0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, -1000, 0);
	v.color.Set (0,1,0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 1000, 0);
	v.color.Set (0,1,0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, -1000);
	v.color.Set (0,0,1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 1000);
	v.color.Set (0,0,1);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (unsigned int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(i);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}

Mesh* MeshBuilder::GenerateLine(const std::string &meshName, Color color, float length)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	v.pos.Set(0, 0, -0.5 * length);
	v.color = color;
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0.5 * length);
	v.color = color;
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	unsigned int index[2] ={
		1,0
	};

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(index[i]);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}
/******************************************************************************/
/*!
\brief
Generate the vertices of a quad; Use color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param color - color of quad
\param lengthX - length of quad
\param lengthZ - width of quad

\return Pointer to mesh storing VBO/IBO of quad
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateQuad(const std::string &meshName, Color color, float lengthX, float lengthZ, int repeat)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	v.pos.Set(-0.5f * lengthX, 0.0f, 0.5f * lengthZ);
	v.color = color;
	v.normal.Set(0,1,0);
	v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.0f, 0.5f * lengthZ);
	v.color = color;
	v.normal.Set(0,1,0);
	v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0.0f, -0.5f * lengthZ);
	v.color = color;
	v.normal.Set(0,1,0);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.0f, -0.5f * lengthZ);
	v.color = color;
	v.normal.Set(0,1,0);
	v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	
	std::vector<GLuint> index_buffer_data;

	unsigned int index[6] ={
		0,1,2,2,1,3
	};

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(index[i]);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateXYQuad(const std::string &meshName, Color color, float lengthX, float lengthY, int repeat)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(1*repeat, 0);

	vertex_buffer_data.push_back(v);
	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	unsigned int index[6] ={
		3,0,2,1,2,0
	};

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(index[i]);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateXYQuadOnPlane(const std::string &meshName, Color color, float lengthX, float lengthY, int repeat)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	v.pos.Set(-0.5f * lengthX, 0 * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0 * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(1*repeat, 0);

	vertex_buffer_data.push_back(v);
	v.pos.Set(0.5f * lengthX, 1 * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 1 * lengthY, 0.0f);
	v.color = color;
	v.normal.Set(0,0,1);
	v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	unsigned int index[6] ={
		3,0,2,1,2,0
	};

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(index[i]);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a cube; Use color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param color - color of cube
\param lengthX - width of cube
\param lengthY - height of cube
\param lengthZ - depth of cube

\return Pointer to mesh storing VBO/IBO of cube
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateCube(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeat)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	//TOP
	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	//BOTTOM
	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	//RIGHT
	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(1,0,0); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(1,0,0); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(1,0,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(1,0,0); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	//LEFT
	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(-1,0,0); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(-1,0,0); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(-1,0,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(-1,0,0); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	//FRONT
	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0,0,1); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	//BACK
	v.pos.Set(-0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,0,-1); v.texCoord.Set(0, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,0,-1); v.texCoord.Set(1*repeat, 1*repeat);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,0,-1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, -0.5f * lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0,0,-1); v.texCoord.Set(1*repeat, 0);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(0 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(3 + (i * 4));
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateBlock(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU, int repeatV)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	//TOP
	v.pos.Set(0,  lengthY,  lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(1* repeatU, 1* repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX,  lengthY,  lengthZ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(0* repeatU, 1* repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0 ,  lengthY, 0 ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(1* repeatU, 0* repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set( lengthX,  lengthY, 0 ); v.color = color; v.normal.Set(0,1,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BOTTOM
	v.pos.Set( lengthX, 0 ,  lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(1 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0 , 0 ,  lengthZ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(0 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set( lengthX, 0 , 0 ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(1 * repeatU, 0 * repeatV);
	vertex_buffer_data.push_back(v);
	
	v.pos.Set(0 , 0 , 0 ); v.color = color; v.normal.Set(0,-1,0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//RIGHT
	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1 * repeatU, 0 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//LEFT
	v.pos.Set(0, lengthY, lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1 * repeatU, 0 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//FRONT
	v.pos.Set(lengthX, lengthY, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1 * repeatU, 0 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BACK
	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0 * repeatU, 1 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1 * repeatU, 0 * repeatV);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(0 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(3 + (i * 4));
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateCubeOnPlane(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU, int repeatV)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	//TOP
	v.pos.Set(-0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BOTTOM
	v.pos.Set(0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//RIGHT
	v.pos.Set(0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//LEFT
	v.pos.Set(-0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//FRONT
	v.pos.Set(0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, lengthY, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, 0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BACK
	v.pos.Set(-0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, lengthY, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(-0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0.5f * lengthX, 0, -0.5f * lengthZ); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(0 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(3 + (i * 4));
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateStair(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU, int repeatV)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	//TOP UP
	v.pos.Set(0, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(1, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 0.5f);
	vertex_buffer_data.push_back(v);

	//TOP DOWN
	v.pos.Set(0, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(1, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BOTTOM
	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//RIGHT TALL
	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0.5f, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ * 0.5f); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0.5f, 0);
	vertex_buffer_data.push_back(v);

	//RIGHT SHORT
	v.pos.Set(lengthX, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(1, 0, 0);  v.texCoord.Set(0.5f, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ * 0.5f); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0.5f, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(1, 0, 0);  v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//LEFT TALL
	v.pos.Set(0, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0.5f, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ * 0.5f); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0.5f, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//LEFT SHORT
	v.pos.Set(0, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0.5f, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ * 0.5f); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0.5f, 0);
	vertex_buffer_data.push_back(v);

	//FRONT UP
	v.pos.Set(lengthX, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY * 0.5f, lengthZ * 0.5f); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 0.5f);
	vertex_buffer_data.push_back(v);

	//FRONT DOWN
	v.pos.Set(lengthX, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY * 0.5f, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 0.5f);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	//BACK
	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0, 1);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0, 0);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 10; i++)
	{
		index_buffer_data.push_back(0 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(3 + (i * 4));
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}


Mesh* MeshBuilder::GenerateGrassBlock(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU, int repeatV)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	//TOP
	v.pos.Set(0, lengthY, lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(2, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, lengthZ); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(2, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(0, 1, 0); v.texCoord.Set(1 , 0);
	vertex_buffer_data.push_back(v);

	//BOTTOM
	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(3, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(2 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(3, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, -1, 0); v.texCoord.Set(2 , 0);
	vertex_buffer_data.push_back(v);

	//RIGHT
	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(1, 0, 0); v.texCoord.Set(0 , 0);
	vertex_buffer_data.push_back(v);

	//LEFT
	v.pos.Set(0, lengthY, lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(-1, 0, 0); v.texCoord.Set(0 , 0);
	vertex_buffer_data.push_back(v);

	//FRONT
	v.pos.Set(lengthX, lengthY, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, lengthY, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, lengthZ); v.color = color; v.normal.Set(0, 0, 1); v.texCoord.Set(0 , 0);
	vertex_buffer_data.push_back(v);

	//BACK
	v.pos.Set(0, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, lengthY, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0 , 1  );
	vertex_buffer_data.push_back(v);

	v.pos.Set(0, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(1, 0);
	vertex_buffer_data.push_back(v);

	v.pos.Set(lengthX, 0, 0); v.color = color; v.normal.Set(0, 0, -1); v.texCoord.Set(0 , 0);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 6; i++)
	{
		index_buffer_data.push_back(0 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(2 + (i * 4));
		index_buffer_data.push_back(1 + (i * 4));
		index_buffer_data.push_back(3 + (i * 4));
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateCubeOutline(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	for (unsigned i = 0; i < 2; ++i)
	{
		v.pos.Set(0, i * lengthY, lengthZ); v.color = color; 
		vertex_buffer_data.push_back(v);
		v.pos.Set(lengthX, i * lengthY, lengthZ); v.color = color; 
		vertex_buffer_data.push_back(v);

		v.pos.Set(0, i * lengthY, 0); v.color = color;
		vertex_buffer_data.push_back(v);
		v.pos.Set(lengthX, i * lengthY, 0); v.color = color;
		vertex_buffer_data.push_back(v);
	}

	std::vector<GLuint> index_buffer_data;

	int offset = 0;
	for (unsigned i = 0; i < 2; ++i)
	{
		index_buffer_data.push_back(0 + offset);
		index_buffer_data.push_back(1 + offset);
		index_buffer_data.push_back(2 + offset);
		index_buffer_data.push_back(3 + offset);
		index_buffer_data.push_back(0 + offset);
		index_buffer_data.push_back(2 + offset);
		index_buffer_data.push_back(1 + offset);
		index_buffer_data.push_back(3 + offset);

		offset += 4;
	}

	index_buffer_data.push_back(0);
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(7);

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_LINES;

	return mesh;
}
/******************************************************************************/
/*!
\brief
Generate the vertices of a circle; Use color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param color - color of circle
\param radius - radius of circle

\return Pointer to mesh storing VBO/IBO of circle
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateCircle(const std::string &meshName, Color color, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;

	for (float i = 0.0f; i < 360.0f; i += 10.0f)
	{
		v.pos.Set(radius * sin(Math::DegreeToRadian(i)) / 2, radius * cos(Math::DegreeToRadian(i)) / 2, 0);
		v.color.Set(1, 1, 1);
		v.texCoord.Set(sin(Math::DegreeToRadian(i)) / 2 + 0.5f, cos(Math::DegreeToRadian(i)) / 2 + 0.5f);
		vertex_buffer_data.push_back(v);
	}

	v.pos.Set(0.0f, 0.0f, 0.0f);
	v.color.Set(1, 1, 1);
	v.texCoord.Set(0.5f, 0.5f);
	vertex_buffer_data.push_back(v);

	std::vector<GLuint> index_buffer_data;

	for (int i = 0; i < 36; i++)
	{
		if (i % 2 == 0)
		{
			index_buffer_data.push_back(i);
			index_buffer_data.push_back(36);
		}
		else
		{
			index_buffer_data.push_back(36);
			index_buffer_data.push_back(i);
		}

		if (i == 35)
			index_buffer_data.push_back(0);
		else
			index_buffer_data.push_back(i + 1);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

float sphereX(float phi, float theta)
{
	return cos(Math::DegreeToRadian(phi)) * cos(Math::DegreeToRadian(theta));
}

float sphereY(float phi)
{
	return sin(Math::DegreeToRadian(phi));
}

float sphereZ(float phi, float theta)
{
	return cos(Math::DegreeToRadian(phi)) * sin(Math::DegreeToRadian(theta));
}

float cylinderX(float theta)
{
	return cos(Math::DegreeToRadian(theta));
}

float cylinderZ(float theta)
{
	return sin(Math::DegreeToRadian(theta));
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a sphere; Use color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param color - color of cube
\param numStack - number of vertical components(stacks) of sphere
\param numSlice - number of horizontal components(slices) of sphere
\param radius - radius of sphere

\return Pointer to mesh storing VBO/IBO of sphere
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateSphere(const std::string &meshName, Color color, unsigned numStack, unsigned numSlice, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	

	float degreePerStack = 180.0f / numStack;
	float degreePerSlice = 360.0f / numSlice;

	for (unsigned stack = 0; stack <= numStack; stack++)
	{
		float phi = -90 + stack * degreePerStack;

		for (unsigned slice = 0; slice <= numSlice ; slice++)
		{
			v.color = color;
			float theta = slice * degreePerSlice;
			v.pos.Set(radius * sphereX(phi,theta), radius * sphereY(phi), radius * sphereZ(phi,theta));
			v.normal.Set(sphereX(phi,theta), sphereY(phi), sphereZ(phi,theta));
			vertex_buffer_data.push_back(v);
			
		}
	}

	std::vector<GLuint> index_buffer_data;

	for (unsigned stack = 0; stack < numStack; stack++)
	{
		for (unsigned slice = 0; slice < numSlice; slice++)
		{
			index_buffer_data.push_back(stack * (numSlice + 1) + slice);
			index_buffer_data.push_back((stack + 1) * (numSlice + 1) + slice);
		}
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

/******************************************************************************/
/*!
\brief
Generate the vertices of a cylinder; Use color for each vertex
Then generate the VBO/IBO and store them in Mesh object

\param meshName - name of mesh
\param color - color of cylinder
\param numSlice - number of sides the cylinder has
\param height - height of cylinder
\param radius - radius of cylinder

\return Pointer to mesh storing VBO/IBO of cylinder
*/
/******************************************************************************/
Mesh* MeshBuilder::GenerateCylinder(const std::string &meshName, Color color, unsigned numSlice, float height, float radius)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;	
	std::vector<GLuint> index_buffer_data;

	float degreePerSlice = 360.0f / numSlice;
	
	for (unsigned stack = 0; stack <= 1; stack++)
	{
		for (unsigned slice = 0; slice <= numSlice ; slice++)
		{
			float theta = slice * degreePerSlice;
			v.pos.Set(radius *cylinderX(theta), -height/2 + stack * height, radius * cylinderZ(theta)); 
			v.color = color;
			v.normal.Set(cylinderX(theta), 0, cylinderZ(theta));
			vertex_buffer_data.push_back(v);
		}
	}

	int core1 = vertex_buffer_data.size();
	v.pos.Set(0,( -height / 2) + 1 * height, 0);
	v.color = color;
	v.normal.Set(0, 1, 0);
	vertex_buffer_data.push_back(v);
	
	int core0 = vertex_buffer_data.size();
	v.pos.Set(0,( -height / 2) + 0 * height , 0);
	v.color = color;
	v.normal.Set(0, -1, 0);
	vertex_buffer_data.push_back(v);

	unsigned circle1 = vertex_buffer_data.size();
	for (unsigned slice = 0; slice <= numSlice ; slice++)
	{
		float theta = slice * degreePerSlice;
		v.pos.Set(radius *cylinderX(theta), -height/2 + 1 * height, radius * cylinderZ(theta)); 
		v.color = color;
		v.normal.Set(0,1,0);
		vertex_buffer_data.push_back(v);
	}

	unsigned circle0 = vertex_buffer_data.size();
	for (unsigned slice = 0; slice <= numSlice ; slice++)
	{
		float theta = slice * degreePerSlice;
		v.pos.Set(radius *cylinderX(theta), -height/2 + 0 * height, radius * cylinderZ(theta)); 
		v.color = color;
		v.normal.Set(0,-1,0);
		vertex_buffer_data.push_back(v);
	}

	for (unsigned i = circle0; i < vertex_buffer_data.size(); i++)
	{
		index_buffer_data.push_back(core0);
		index_buffer_data.push_back(i);
	}

	for (unsigned slice = 0; slice <= numSlice; slice++)
	{
		index_buffer_data.push_back(slice);
		index_buffer_data.push_back((numSlice + 1) + slice);
	} 

	for (unsigned i = circle1 ; i < circle0; i++)
	{
		index_buffer_data.push_back(i);
		index_buffer_data.push_back(core1);
	}

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLE_STRIP;

	return mesh;
}

SpriteAnimation* MeshBuilder::GenerateSpriteAnimation(const std::string&meshName, unsigned numRow, unsigned numCol, int w, int h)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data; 

	float width = 1.f / numCol;
	float height = 1.f / numRow;

	int offset = 0;
	
	for(int i = numRow - 1; i >= 0; --i)
	{
		for(unsigned j = 0; j < numCol; ++j)
		{
			v.pos.Set(-0.5f*w, -0.5f*h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width, (float)i * height);
			vertex_buffer_data.push_back(v);

			v.pos.Set(-0.5f*w, 0.5f*h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width, (float)i * height + height);
			vertex_buffer_data.push_back(v);

			v.pos.Set(0.5f*w, 0.5f*h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j  * width + width, (float)i * height + height);
			vertex_buffer_data.push_back(v);
			
			v.pos.Set(0.5f*w, -0.5f*h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width + width, (float)i * height);
			vertex_buffer_data.push_back(v);

			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 1);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 3);

			offset+=4;
		}
	}

	SpriteAnimation *mesh = new SpriteAnimation(meshName, numRow, numCol, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

SpriteAnimation* MeshBuilder::GenerateSpriteAnimationOnPlane(const std::string&meshName, unsigned numRow, unsigned numCol, int w, int h)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data; 

	float width = 1.f / numCol;
	float height = 1.f / numRow;

	int offset = 0;
	
	for(int i = numRow - 1; i >= 0; --i)
	{
		for(unsigned j = 0; j < numCol; ++j)
		{
			v.pos.Set(-0.5f*w, 0, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width, (float)i * height);
			vertex_buffer_data.push_back(v);

			v.pos.Set(-0.5f*w, h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width, (float)i * height + height);
			vertex_buffer_data.push_back(v);

			v.pos.Set(0.5f*w, h, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j  * width + width, (float)i * height + height);
			vertex_buffer_data.push_back(v);
			
			v.pos.Set(0.5f*w, 0, 0);
			v.normal.Set(0,0,1);
			v.texCoord.Set((float)j * width + width, (float)i * height);
			vertex_buffer_data.push_back(v);

			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 1);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 3);

			offset+=4;
		}
	}

	SpriteAnimation *mesh = new SpriteAnimation(meshName, numRow, numCol, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}

Mesh* MeshBuilder::GenerateOBJ(const std::string &meshName, const std::string &file_path)
{
	//Read vertices, texcoords & normals from OBJ
	std::vector<Position> vertices;
	std::vector<TexCoord> uvs;
	std::vector<Vector3> normals;
	bool success = LoadOBJ(file_path.c_str(), vertices, uvs, normals);
	if(!success)
		return NULL;
	//Index the vertices, texcoords & normals properly
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;
	IndexVBO(vertices, uvs, normals, index_buffer_data, vertex_buffer_data);

	Mesh *mesh = new Mesh(meshName, vertex_buffer_data, index_buffer_data);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = Mesh::DRAW_TRIANGLES;

	return mesh;
}