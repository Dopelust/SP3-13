#ifndef LIVING_H
#define LIVING_H

#include "Entity.h"

class Living : public Entity
{
public:
	Living();
	~Living();

	unsigned id;
	virtual void Update(double dt, bool RestrictMovement);
};

class Arrow : public Living
{
public:
	Arrow();
	~Arrow();

	void Update(double dt, bool RestrictMovement);
	void RespondToCollision(const vector<Block*>&object);
};
#endif