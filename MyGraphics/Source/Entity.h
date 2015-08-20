#ifndef ENTITY_H
#define ENTITY_H

#include "Vector3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Block.h"

#include <string>
#include <vector>

#define NumEntities 5 //Enemy1, Enemy2, Enemy3, Horse, Arrow

using namespace::std;

class Arrow;

class Entity
{
public:
	Entity();
	virtual ~Entity();

	Mesh * mesh;
	Vector3 initialVel;
	Vector3 initialPos;
	Vector3 position;
	Vector3 velocity;
	Collision collision;
	vector<Block*> collisionBlockList;

	float hOrientation;
	float vOrientation;

	float lifetime;
	Vector3 size;
	bool showOnMinimap;
	bool jump;

	bool IsActive();
	void SetActive(bool active);

	bool IsDead();
	void SetDead(bool dead);

	virtual void Update(double dt);
	virtual void Update(double dt, const vector<Block*>&object, bool RestrictMovement);
	virtual void RespondToCollision(const vector<Block*>&object);
	virtual void RenderObject(MS& modelStack);

	float climbHeight;

	void Knockback(Vector3 dir);
	vector<Arrow*> StuckedArrows;
protected:
	bool active;
	bool dead;
	bool sneak;
	float stepRate;
	char* stepSound;
};

#endif