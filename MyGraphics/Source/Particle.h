#ifndef PARTICLE_H
#define PARTICLE_H

#include "Entity.h"

class Particle : public Entity
{
public:
	enum particleType
	{
		BLOCK,
		ORB,
		NUM_PARTICLES,
	};

	Particle();
	~Particle();

	Color color;
	unsigned blockID;
	virtual void Update(double dt);

	unsigned particleID;
	virtual unsigned getSubID();

	friend bool pIsMoreThan(const Particle*, const Particle*);
};

class Orb : public Particle
{
public:
	enum orbType
	{
		HEALTH,
		EXPERIENCE,
		ZONE,
		NUM_ORBS,
	};

	Orb();
	~Orb();

	unsigned orbID;
	void Update(double dt);
	Entity* target;

	unsigned getSubID();
};

#endif