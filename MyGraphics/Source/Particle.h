#ifndef PARTICLE_H
#define PARTICLE_H

#include "Entity.h"

class Particle : public Entity
{
public:
	Particle();
	~Particle();

	Color color;
	unsigned blockID;
	void Update(double dt);
};

#endif