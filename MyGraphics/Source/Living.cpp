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

	velocity.x += -velocity.x * dt * 16;
	velocity.z += -velocity.z * dt * 16;

	if (velocity.x > -0.1f && velocity.x < 0.1f)
		velocity.x = 0;
	if (velocity.z > -0.1f && velocity.z < 0.1f)
		velocity.z = 0;

	velocity.y -= 30 * dt;

	position += velocity * dt;
	RespondToCollision(this->collisionBlockList);

	unsigned count = StuckedArrows.size();
	for (unsigned i = 0; i < count; ++i)
	{
		StuckedArrows[i]->position = position + StuckedArrowsRelativePosition[i];
	}
}

Arrow::Arrow()
{
}

Arrow::~Arrow()
{
}

void Arrow::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	position += velocity * dt;
	velocity.y -= 16 * dt;

	Vector3 dir = velocity.Normalized();

	if (dir.y < 0)
		vOrientation = Math::RadianToDegree(atan(sqrt(dir.x * dir.x + dir.z * dir.z) / -dir.y)) - 90;
	else
		vOrientation = Math::RadianToDegree(atan(sqrt(dir.x * dir.x + dir.z * dir.z) / -dir.y)) + 90;

	RespondToCollision(this->collisionBlockList);
}

#define eps 0.000001f;

void Arrow::RespondToCollision(const vector<Block*>& object)
{
	vector<Block> collidedBlocks;
	vector<float> penetrationArea;

	Block Player(position, collision.centre, collision.hitbox);

	unsigned count = object.size();
	for (unsigned i = 0; i < count; ++i)
	{
		if (object[i]->type == Block::STAIR)
		{
			for (unsigned j = 0; j < object[i]->collisions.size(); ++j)
			{
				Block Blk(object[i]->position, object[i]->collisions[j].centre, object[i]->collisions[j].hitbox);
				if (Block::checkCollision(Player, Blk))
					dead = true;
			}
		}
		else
		{
			if (Block::checkCollision(Player, *object[i]))
				dead = true;
		}
	}

}
