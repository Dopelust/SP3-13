#include "Particle.h"

Particle::Particle() : blockID(0)
{
	color.Set((float)(rand()%255)/255.f, (float)(rand()%255)/255.f, (float)(rand()%255)/255.f);
}
Particle::~Particle()
{
}

void Particle::Update(double dt)
{
	position += velocity * dt;

	velocity.y -= 16.f * dt;
	lifetime += dt;

	if (lifetime >= 0.7f)
		SetActive(false);
}