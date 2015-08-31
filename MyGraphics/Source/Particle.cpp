#include "Particle.h"
#include "MinScene.h"

Particle::Particle() : blockID(0), particleID(Particle::BLOCK)
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

unsigned Particle::getSubID()
{
	return 0;
}

unsigned Orb::getSubID()
{
	return orbID;
}

Orb::Orb() : orbID(Orb::HEALTH)
{
	particleID = Particle::ORB;
}
Orb::~Orb()
{
}

void Orb::Update(double dt)
{
	position += velocity * dt;
	Vector3 dir = Vector3(target->position.x, target->position.y + 1.62f, target->position.z) - position;
	if (dir.LengthSquared() < 1)
		dir.Normalize();
	position +=  dir * 6 * dt;

	velocity += -velocity * 10 * dt;

	velocity.x = velocity.x > -0.1f && velocity.x < 0.1f ? 0 : velocity.x;
	velocity.y = velocity.y > -0.1f && velocity.y < 0.1f ? 0 : velocity.y;
	velocity.z = velocity.z > -0.1f && velocity.z < 0.1f ? 0 : velocity.z;

	lifetime += dt;

	if (lifetime >= 100.f)
		SetActive(false);
}

bool pIsMoreThan(const Particle* lhs, const Particle* rhs)
{
	return SceneBase::camera->position.DistSquared(lhs->position) > SceneBase::camera->position.DistSquared(rhs->position);
}