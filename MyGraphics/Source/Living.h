#ifndef LIVING_H
#define LIVING_H

#include "Entity.h"

class Living : public Entity
{
public:
	Living();
	~Living();

	float hitRate;
	float hitTimer;

	float headBob;
	float newOrientation;

	float timeToStop;
	float timeToGo;

	virtual void Update(double dt, bool RestrictMovement);
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