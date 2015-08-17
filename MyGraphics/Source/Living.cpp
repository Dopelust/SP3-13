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

	velocity.SphericalToCartesian(hOrientation, 0); velocity.y = initialVel.y;
	position += velocity * dt;
	velocity.y -= 30 * dt;

	RespondToCollision(this->collisionBlockList);
}
