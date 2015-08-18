#include "Living.h"

Living::Living()
{
}

Living::~Living()
{
}

void Living::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	position += velocity * dt;
	velocity.x += -velocity.x * dt * 16;
	velocity.z += -velocity.z * dt * 16;
	if (velocity.x > -0.1f && velocity.x < 0.1f)
		velocity.x = 0;
	if (velocity.z > -0.1f && velocity.z < 0.1f)
		velocity.z = 0;
	velocity.y -= 30 * dt;

	RespondToCollision(this->collisionBlockList);
}

Horse::Horse()
{
	climbHeight = 1;
}

Horse::~Horse()
{
}