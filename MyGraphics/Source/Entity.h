#ifndef ENTITY_H
#define ENTITY_H

#include "Vector3.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Block.h"

#include <string>
#include <vector>

#define NumEntities 8

using namespace::std;

class Arrow;

class Entity
{
public:
	enum EntityIDs
	{
		SENTRY,
		KNIGHT,
		NPC,
		HORSE,
		ARROW,
		WOLF,
		DROP,
		KEY,
		NUM_ENTITIES,
	};

	Entity();
	virtual ~Entity();

	Mesh * mesh;
	Vector3 initialVel;
	Vector3 initialPos;
	Vector3 position;
	Vector3 velocity;
	Vector3 kbVelocity;
	Collision collision;
	vector<Block*> collisionBlockList;

	unsigned entityID;
	float headOrientation;
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

	virtual bool IsLiving();

	virtual void Update(double dt);
	virtual void Update(double dt, bool RestrictMovement);
	virtual void RespondToCollision(const vector<Block*>&object);
	virtual void RenderObject(MS& modelStack);
	virtual bool canAttack();
	virtual void Attack();
	virtual string getSpeech();
	virtual unsigned getSubID();

	void WorldBorderCheck();

	float climbHeight;

	Entity* mount;
	Entity* aggro;
	virtual float getSkeletalRotation();
	void Knockback(Vector3 dir);
	vector<Arrow*> StuckedArrows;
	void ClearArrows();

	float Steps;
	float health;
	float viewRange;
	float maxViewRange;
protected:
	bool active;
	bool dead;
	bool sneak;
	float stepRate;
	char* stepSound;
};

class Key : public Entity
{
public:
	Key();
	~Key();
	void Update(double dt, bool RestrictMovement);
};

#endif