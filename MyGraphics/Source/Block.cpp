/******************************************************************************/
/*!
\file	collision.cpp
\author Ricsson
\par	
\brief
This is the collision.cpp that contains all the functions and constructors.
*/
/******************************************************************************/
#include "Block.h"
#include "MinScene.h"
#include "Application.h"

Block::Block() : position(0,0,0), id(0), ignoreCollision(false), type(DEFAULT)
{
}
Block::Block(unsigned id, blockType type)
{
	*this = Block();
	this->id = id;
	this->type = type;
}
Block::Block(Vector3 p, Vector3 c, Vector3 h)
{
	*this = Block();
	position = p;
	collision.Set(c, h);
}
Block::~Block()
{
}

/******************************************************************************/
/*!
		Class Block:
\brief	Collsion checker function
*/
/******************************************************************************/
bool Block::checkCollision(Block& a, Block&  b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	return(maxCubeA.x > minCubeB.x && 
		minCubeA.x < maxCubeB.x && 
		maxCubeA.y > minCubeB.y && 
		minCubeA.y < maxCubeB.y && 
		maxCubeA.z > minCubeB.z && 
		minCubeA.z < maxCubeB.z);
}
bool Block::collisionWithSphere(Vector3 point, float radius)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	float dist_squared = radius * radius;

    if (point.x < minCube.x) 
		dist_squared -= (point.x - minCube.x) * (point.x - minCube.x);
    else if (point.x > maxCube.x) 
		dist_squared -= (point.x - maxCube.x) * (point.x - maxCube.x);
    if (point.y < minCube.y) 
		dist_squared -= (point.y - minCube.y) * (point.y - minCube.y);
    else if (point.y > maxCube.y) 
		dist_squared -= (point.y - maxCube.y) * (point.y - maxCube.y);
    if (point.z < minCube.z)
		dist_squared -= (point.z - minCube.z) * (point.z - minCube.z);
    else if (point.z > maxCube.z) 
		dist_squared -= (point.z - maxCube.z) * (point.z - maxCube.z);

    return dist_squared > 0;
}
bool Block::collisionWithCylinder(Vector3 point, float radius)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	float dist_squared = radius * radius;

	if (point.x < minCube.x)
		dist_squared -= (point.x - minCube.x) * (point.x - minCube.x);
	else if (point.x > maxCube.x)
		dist_squared -= (point.x - maxCube.x) * (point.x - maxCube.x);
	if (point.z < minCube.z)
		dist_squared -= (point.z - minCube.z) * (point.z - minCube.z);
	else if (point.z > maxCube.z)
		dist_squared -= (point.z - maxCube.z) * (point.z - maxCube.z);

	return dist_squared > 0;
}
float Block::collisionWithRay(Vector3 point, Vector3 dir)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	// r.dir is unit direction vector of ray
	Vector3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;

	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (minCube.x - point.x)*dirfrac.x;
	float t2 = (maxCube.x - point.x)*dirfrac.x;
	float t3 = (minCube.y - point.y)*dirfrac.y;
	float t4 = (maxCube.y - point.y)*dirfrac.y;
	float t5 = (minCube.z - point.z)*dirfrac.z;
	float t6 = (maxCube.z - point.z)*dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
		return -1;
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
		return -1;

	return tmin;
}
int Block::RayCollisionFace(Vector3 point, Vector3 dir)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	// r.dir is unit direction vector of ray
	Vector3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;

	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (minCube.x - point.x)*dirfrac.x;
	float t2 = (maxCube.x - point.x)*dirfrac.x;
	float t3 = (minCube.y - point.y)*dirfrac.y;
	float t4 = (maxCube.y - point.y)*dirfrac.y;
	float t5 = (minCube.z - point.z)*dirfrac.z;
	float t6 = (maxCube.z - point.z)*dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));

	if (tmin == t1) 
		return 1;
	if (tmin == t2)
		return 2;
	if (tmin == t3)
		return 3;
	if (tmin == t4) 
		return 4;
	if (tmin == t5)
		return 5;
	if (tmin == t6)
		return 6;

	return 0;
}
float Block::PenetrationDepth(Block& a, Block&  b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	float xPenetrationDepth = 0.f;
	float yPenetrationDepth = 0.f;
	float zPenetrationDepth = 0.f;

	if (maxCubeA.x > maxCubeB.x && minCubeA.x > minCubeB.x)
		xPenetrationDepth = maxCubeB.x - minCubeA.x;

	else if (maxCubeA.x < maxCubeB.x && minCubeA.x < minCubeB.x)
		xPenetrationDepth = maxCubeA.x - minCubeB.x;

	else if (minCubeA.x >= minCubeB.x && maxCubeA.x <= maxCubeB.x)
		xPenetrationDepth = maxCubeA.x - minCubeA.x;

	else if (minCubeA.x <= minCubeB.x && maxCubeA.x >= maxCubeB.x)
		xPenetrationDepth = maxCubeA.x - minCubeA.x;

	if (maxCubeA.y > maxCubeB.y && minCubeA.y > minCubeB.y)
		yPenetrationDepth = maxCubeB.y - minCubeA.y;

	else if (maxCubeA.y < maxCubeB.y && minCubeA.y < minCubeB.y)
		yPenetrationDepth = maxCubeA.y - minCubeB.y;

	else if (minCubeA.y >= minCubeB.y && maxCubeA.y <= maxCubeB.y)
		yPenetrationDepth = maxCubeA.y - minCubeA.y;

	else if (minCubeA.y <= minCubeB.y && maxCubeA.y >= maxCubeB.y)
		yPenetrationDepth = maxCubeA.y - minCubeA.y;

	if (maxCubeA.z > maxCubeB.z && minCubeA.z > minCubeB.z)
		zPenetrationDepth = maxCubeB.z - minCubeA.z;

	else if (maxCubeA.z < maxCubeB.z && minCubeA.z < minCubeB.z)
		zPenetrationDepth = maxCubeA.z - minCubeB.z;

	else if (minCubeA.z >= minCubeB.z && maxCubeA.z <= maxCubeB.z)
		zPenetrationDepth = maxCubeA.z - minCubeA.z;

	else if (minCubeA.z <= minCubeB.z && maxCubeA.z >= maxCubeB.z)
		zPenetrationDepth = maxCubeA.z - minCubeA.z;

	return xPenetrationDepth * yPenetrationDepth * zPenetrationDepth;
}

void Block::Update(double dt)
{
}

void Block::RenderObject(MS& modelStack)
{
	modelStack.Translate(position);
}

Vector3 Block::getMaxCoord()
{
	return position + collision.centre + collision.hitbox * 0.5f;
}
Vector3 Block::getMinCoord()
{
	return position + collision.centre - collision.hitbox * 0.5f;
}

TexCoord Block::getTextureOffset(unsigned blockID, blockType type)
{
	if (type == MT)
	{
		float column = ( (blockID*3) % 6) / 6.f;
		float row = 1 - (((blockID * 3) / 6) + 1) / 6.f;

		return TexCoord(column, row);
	}

	blockID -= 4;
	float column = (blockID % 4) / 4.f;
	float row = 1 - ((blockID / 4)+ 1) / 4.f;

	return TexCoord(column, row);
}

char* Block::getSound(unsigned blockID)
{
	switch (blockID)
	{
	case 0: case 1: case 13:
		if (rand() % 4)
			return "Assets/Media/Block/grass1.mp3";
		if (rand() % 3)
			return "Assets/Media/Block/grass2.mp3";
		if (rand() % 2)
			return "Assets/Media/Block/grass3.mp3";
		return "Assets/Media/Block/grass4.mp3";
	case 2: 
		if (rand() % 2)
			return "Assets/Media/Block/wood3.mp3";
		return "Assets/Media/Block/wood4.mp3";
	case 3: return "Assets/Media/Block/stone.mp3";
	case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
		return "Assets/Media/Block/stone.mp3";
	case 12: 
		if (rand() % 4)
			return "Assets/Media/Block/gravel1.mp3";
		if (rand() % 3)
			return "Assets/Media/Block/gravel2.mp3";
		if (rand() % 2)
			return "Assets/Media/Block/gravel3.mp3";
		return "Assets/Media/Block/gravel4.mp3";
	}

	return NULL;
}

char* Block::getStepSound(unsigned blockID)
{
	switch (blockID)
	{
	case 0: case 1: case 13:
		if (rand() % 6)
			return "Assets/Media/Block/Step/grass1.mp3";
		if (rand() % 5)
			return "Assets/Media/Block/Step/grass2.mp3";
		if (rand() % 4)
			return "Assets/Media/Block/Step/grass3.mp3";
		if (rand() % 3)
			return "Assets/Media/Block/Step/grass4.mp3";
		if (rand() % 2)
			return "Assets/Media/Block/Step/grass5.mp3";
		return "Assets/Media/Block/Step/grass6.mp3";
	case 2:
		if (rand() % 2)
			return "Assets/Media/Block/wood3.mp3";
		return "Assets/Media/Block/wood4.mp3";
	case 3: return "Assets/Media/Block/stone.mp3";
	case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
		if (rand() % 4)
			return "Assets/Media/Block/Step/stone1.mp3";
		if (rand() % 3)
			return "Assets/Media/Block/Step/stone2.mp3";
		if (rand() % 2)
			return "Assets/Media/Block/Step/stone3.mp3";
		return "Assets/Media/Block/Step/stone4.mp3";
	case 12:
		if (rand() % 4)
			return "Assets/Media/Block/gravel1.mp3";
		if (rand() % 3)
			return "Assets/Media/Block/gravel2.mp3";
		if (rand() % 2)
			return "Assets/Media/Block/gravel3.mp3";
		return "Assets/Media/Block/gravel4.mp3";
	}

	return NULL;
}

bool IsMoreThan(const Block* lhs, const Block* rhs)
{
	return SceneBase::camera->position.DistSquared(lhs->position) > SceneBase::camera->position.DistSquared(rhs->position);
}
bool Block::operator<(Block & rhs)
{
	return false;	return SceneBase::camera->position.DistSquared(position) < SceneBase::camera->position.DistSquared(rhs.position);
}

void Block::PrintData(ostream& os)
{
	os << MinScene::PositionToIndex(position) << " " << id << " " << type << endl;
}

Stair::Stair() : orientation(0), look(0)
{
}

Stair::~Stair()
{
}

void Stair::RenderObject(MS& modelStack)
{
	modelStack.Translate(position);
	if (orientation != 0)
	{
		modelStack.Translate(collision.centre.x, 0, collision.centre.z);
		modelStack.Rotate(orientation, 0, 1, 0);
		modelStack.Translate(-collision.centre.x, 0, -collision.centre.z);
	}
	if (look != 0)
	{
		modelStack.Translate(collision.centre);
		modelStack.Rotate(look, 1, 0, 0);
		modelStack.Translate(-collision.centre);
	}
}

void Stair::PrintData(ostream& os)
{
	os << MinScene::PositionToIndex(position) << " " << id << " "  << type << " " << orientation << " " << look << endl;
}

ostream& operator<<(ostream& os, Block& block)
{
	block.PrintData(os);
	return os;
}

BlockFactory::BlockFactory() : selectedBlock(0), size(0)
{
}

BlockFactory::~BlockFactory()
{
}

void BlockFactory::Init()
{
	for (unsigned i = 0; i < 4; ++i)
		blockList.push_back(Block(i, Block::MT));

	for (unsigned i = 4; i < NumBlocks; ++i)
	{
		if (i == 13)
			blockList.push_back(Block(i, Block::TRANS));
		else
		{
			blockList.push_back(Block(i, Block::DEFAULT));

			if (i != 9)
				blockList.push_back(Block(i, Block::STAIR));
		}
	}

	size = blockList.size();
}

void BlockFactory::Update()
{
	if (!blockList.empty())
	{
		if (Application::mouseScroll != 0)
		{
			selectedBlock -= Application::mouseScroll;
			Application::mouseScroll = 0;
		}

		if (selectedBlock < 0)
			selectedBlock = size - 1;
		else if (selectedBlock >= size)
			selectedBlock = 0;
	}
}

Block& BlockFactory::getBlock(unsigned index)
{
	if (index < size)
		return blockList[index];

	return Block();
}


Block& BlockFactory::getBlock()
{
	return blockList[selectedBlock];
}

void BlockFactory::setBlock(const Block& block)
{
	for (unsigned i = 0; i < size; ++i)
	{
		if (blockList[i].id == block.id && blockList[i].type == block.type)
		{
			selectedBlock = i;
			break;
		}
	}
}