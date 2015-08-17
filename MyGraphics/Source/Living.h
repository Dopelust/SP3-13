#ifndef LIVING_H
#define LIVING_H

#include "Entity.h"

class Living : public Entity
{
public:
	Living();
	~Living();

	unsigned id;
	void Update(double dt, const vector<Block*>&object, bool RestrictMovement);
};

#endif