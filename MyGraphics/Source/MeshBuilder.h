/******************************************************************************/
/*!
\file	MeshBuilder.h
\author Ricsson Toh
\brief 
Class to generate meshes
*/
/******************************************************************************/

#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "SpriteAnimation.h"
#include "Minimap.h"
#include "Mesh.h"
#include "vertex.h"
#include "LoadOBJ.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName);
	static Mesh* GenerateLine(const std::string &meshName, Color color, float length);
	static Mesh* GenerateRay(const std::string &meshName, Color color, float length);

	static Mesh* GenerateQuad(const std::string &meshName, Color color, float lengthX, float lengthZ, int repeat=1);
	static Mesh* GenerateXYQuad(const std::string &meshName, Color color, float lengthX, float lengthY, int repeat=1);
	static Mesh* GenerateXYQuadOnPlane(const std::string &meshName, Color color, float lengthX, float lengthY, int repeat=1);

	static Mesh* GenerateBlock(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU = 1, int repeatV = 1);
	static Mesh* GenerateCubeOnPlane(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU=1, int repeatV=1);
	static Mesh* GenerateStair(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU = 1, int repeatV = 1);
	static Mesh* GenerateGrassBlock(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeatU = 1, int repeatV = 1);
	static Mesh* GenerateCube(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ, int repeat=1);
	static Mesh* GenerateCubeOutline(const std::string &meshName, Color color, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateCircle(const std::string &meshName, Color color, float radius);
	static Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned numStack, unsigned numSlice, float radius);
	static Mesh* GenerateCylinder(const std::string &meshName, Color color, unsigned numSlice, float height, float radius);

	static SpriteAnimation* MeshBuilder::GenerateSpriteAnimation(const std::string&meshName, unsigned numRow, unsigned numCol, int w=1, int h=1);
	static SpriteAnimation* MeshBuilder::GenerateSpriteAnimationOnPlane(const std::string&meshName, unsigned numRow, unsigned numCol, int w=1, int h=1);

	static Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
};

#endif