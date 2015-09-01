#ifndef LIVING_H
#define LIVING_H

#include "Entity.h"

class NPC : public Entity
{
public:
	NPC();
	~NPC();

	unsigned id;

	unsigned getSubID();
	string getSpeech();
	void Update(double dt, bool RestrictMovement);
};

class Living : public Entity
{
public:
	Living();
	~Living();

	float hitRate;
	float hitTimer;

	float headBob;

	virtual void Update(double dt, bool RestrictMovement);
	virtual void Animate(double dt);
	bool canAttack();
	void Attack();
	
	bool IsLiving();
};

class Wolf : public Living
{
public:
	Wolf();
	~Wolf();

	float newOrientation;

	float timeToStop;
	float timeToGo;

	virtual void Update(double dt, bool RestrictMovement);
	virtual void Animate(double dt);
};

class Knight : public Living
{
public:
	Knight();
	~Knight();

	int orient;
	float orientTimer;
	float originalOrientation;

	bool turnLeft;

	void Update(double dt, bool RestrictMovement);
	void Animate(double dt);
};

class Sentry : public Living
{
public:
	Sentry();
	~Sentry();

	void Update(double dt, bool RestrictMovement);
	void Animate(double dt);
};

class Drop : public Entity
{
public:
	Drop();
	~Drop();

	void Update(double dt, bool RestrictMovement);
	void RespondToCollision(const vector<Block*>&object);
};

class Arrow : public Entity
{
public:
	Arrow();
	~Arrow();

	void Update(double dt, bool RestrictMovement);
	void RespondToCollision(const vector<Block*>&object);

	Vector3 relativePosition;
	float relativeOrientation;
};

class Enemy : public Living
{
private:
	//General variables
	bool turn;
	int state;
	float stepCounter;

	//Idle
	bool MoveTo;
	bool MoveBack;


	//Patrol
	double patrolTime;
	double stopToCheckTime;

	bool MoveTo1;
	bool MoveTo2;
	bool MoveTo3;
	bool MoveTo4;
	bool stopToCheck;
	double stopTimer;
	//Triangle===
	bool turn1;
	bool turn2;
	//Triangle===

public:
	Enemy();
	Enemy(int state, double patrolTime, double stopToCheckTime);
	~Enemy();

	void Update(double dt, bool RestrictMovement);

	void Idle(double dt);
	void Patrol(double dt);
	void PatrolSquare(double dt);
	void PatrolTriangle(double dt);
	void Chase(double dt);

};
#endif