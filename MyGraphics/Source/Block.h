/******************************************************************************/
/*!
\file	Block.h
\author Ricsson
\par	
\brief
This is the collsion class using AABB.
*/
/******************************************************************************/
#ifndef BLOCK_H
#define BLOCK_H
#define NumBlocks 14

#include "vertex.h"
#include "MatrixStack.h"
#include "Vector3.h"
#include <vector>

using namespace::std;

/******************************************************************************/
/*!
		Struct Collision:
\brief	This is a struct that has the positions of the hitBox size and its centre.
*/
/******************************************************************************/
struct Collision
{
	Collision() {};
	Collision(Vector3 c, Vector3 h)
	{
		Set(c, h);
	}
	void Set(Vector3 c, Vector3 h)
	{
		centre = c;
		hitbox = h;
	}
	void Set(Vector3 h)
	{
		hitbox = h;
		centre.Set(0, h.y/2, 0);
	}

	Vector3 centre;
	Vector3 hitbox;
};

/******************************************************************************/
/*!
		Class Block:
\brief	This is the object class that takes in a lot of member variables depending on what is needed
like if there is no mesh or whether or not the object is an item that can be paid for.
*/
/******************************************************************************/
class Block
{
public:
	enum blockType
	{
		DEFAULT,
		MT,
		STAIR,
		TRANS,
		NUM_TYPES,
	};

	Block();
	Block(unsigned id, blockType type);
	Block(Vector3 p, Vector3 c, Vector3 h);
	virtual ~Block();

	Vector3 position;
	unsigned id;
	blockType type;

	/*COLLISION*/
	Collision collision; //For one hitbox
	vector<Collision> collisions; //For multiple hitboxes

	static bool checkCollision(Block& a, Block& b); //AABB - AABB
	bool collisionWithSphere(Vector3 point, float radius); //Sphere - AABB
	bool collisionWithCylinder(Vector3 point, float radius); //Cylinder - AABB
	float collisionWithRay(Vector3 point, Vector3 dir); //Ray - AABB
	int RayCollisionFace(Vector3 point, Vector3 dir);

	static float PenetrationDepth(Block& a, Block& b); //Determines collision order for dynamic objects

	bool ignoreCollision;

	/*FUNCTIONS*/
	Vector3 getMaxCoord();
	Vector3 getMinCoord();
	static TexCoord getTextureOffset(unsigned blockID, blockType type = DEFAULT);
	static char* getSound(unsigned blockID);
	static char* getStepSound(unsigned blockID);

	friend bool IsMoreThan(const Block*, const Block*);
	bool operator< (Block& rhs);

	//Scene Functions
	virtual void Update(double dt);
	virtual void RenderObject(MS& modelStack);

	virtual void PrintData(ostream& os);
	friend ostream& operator<<(ostream& os, Block& block);
};

class Stair : public Block
{
public:
	Stair();
	~Stair();

	int orientation;
	int look;

	void RenderObject(MS& modelStack);
	void PrintData(ostream& os);
};

class BlockFactory : public Block
{
	int selectedBlock;
	vector<Block> blockList;
public:
	BlockFactory();
	~BlockFactory();

	void Init();
	void Update();
	Block& getBlock(unsigned index);
	Block& getBlock();
	void setBlock(const Block& block);

	unsigned size;
};

#endif