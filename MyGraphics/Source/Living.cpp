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
	if (id == 4)
	{
		if (!velocity.IsZero())
		{
			velocity.y -= 16 * dt;
			Vector3 dir = velocity.Normalized();

			if (dir.y < 0)
				vOrientation = Math::RadianToDegree(atan(sqrt(dir.x * dir.x + dir.z * dir.z) / -dir.y)) - 90;
			else
				vOrientation = Math::RadianToDegree(atan(sqrt(dir.x * dir.x + dir.z * dir.z) / -dir.y)) + 90;
		}
		else
		{
			active = false;
			return;
		}
	}
	else
	{
		if (id < 3)
		{
			Vector3 dir; dir.SphericalToCartesian(hOrientation, 0);
			velocity.x = dir.x; velocity.z = dir.z;
		}

		velocity.x += -velocity.x * dt * 16;
		velocity.z += -velocity.z * dt * 16;

		if (velocity.x > -0.1f && velocity.x < 0.1f)
			velocity.x = 0;
		if (velocity.z > -0.1f && velocity.z < 0.1f)
			velocity.z = 0;

		velocity.y -= 30 * dt;
	}

	RespondToCollision(this->collisionBlockList);
}

Arrow::Arrow()
{
}

Arrow::~Arrow()
{
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
				Blk.id = object[i]->id;

				if (Block::checkCollision(Player, Blk))
				{
					float area = Block::PenetrationDepth(Player, Blk);

					if (collidedBlocks.empty())
					{
						collidedBlocks.push_back(Blk);
						penetrationArea.push_back(area);
					}
					else
					{
						unsigned A = penetrationArea.size();

						for (unsigned a = 0; a <= A; ++a)
						{
							if (a == A)
							{
								collidedBlocks.push_back(Blk);
								penetrationArea.push_back(area);
								break;
							}
							else if (area >= penetrationArea[a])
							{
								collidedBlocks.insert(collidedBlocks.begin() + a, Blk);
								penetrationArea.insert(penetrationArea.begin() + a, area);
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			if (Block::checkCollision(Player, *object[i]))
			{
				float area = Block::PenetrationDepth(Player, *object[i]);

				if (collidedBlocks.empty())
				{
					collidedBlocks.push_back(*object[i]);
					penetrationArea.push_back(area);
				}
				else
				{
					unsigned A = penetrationArea.size();

					for (unsigned a = 0; a <= A; ++a)
					{
						if (a == A)
						{
							collidedBlocks.push_back(*object[i]);
							penetrationArea.push_back(area);
							break;
						}
						else if (area >= penetrationArea[a])
						{
							collidedBlocks.insert(collidedBlocks.begin() + a, *object[i]);
							penetrationArea.insert(penetrationArea.begin() + a, area);
							break;
						}
					}
				}
			}
		}
	}

	Vector3 maxPlayer = initialPos + Player.collision.centre + Player.collision.hitbox  * 0.5f;
	Vector3 minPlayer = initialPos + Player.collision.centre - Player.collision.hitbox * 0.5f;

	count = collidedBlocks.size();
	for (unsigned i = 0; i < count; ++i)
	{
		Block P(position, collision.centre, collision.hitbox);
		if (Block::checkCollision(P, collidedBlocks[i]))
		{
			Vector3 maxCube = collidedBlocks[i].getMaxCoord();
			Vector3 minCube = collidedBlocks[i].getMinCoord();

			if (maxPlayer.z >= maxCube.z && minPlayer.z >= maxCube.z)
			{
				velocity.SetZero();
				//position.z = maxCube.z + Player.collision.hitbox.z * 0.5f + eps;
			}
			else if (maxPlayer.z <= minCube.z && minPlayer.z <= minCube.z)
			{
				velocity.SetZero();
				//position.z = minCube.z - Player.collision.hitbox.z * 0.5f - eps;
			}
			else if (maxPlayer.x >= maxCube.x && minPlayer.x >= maxCube.x)
			{
				velocity.SetZero();
				//position.x = maxCube.x + Player.collision.hitbox.x * 0.5f + eps;
			}
			else if (maxPlayer.x <= minCube.x && minPlayer.x <= minCube.x)
			{
				velocity.SetZero();
				//position.x = minCube.x - Player.collision.hitbox.x * 0.5f - eps;
			}
			else if (maxPlayer.y >= maxCube.y && minPlayer.y >= maxCube.y)
			{
				//position.y = maxCube.y + collision.hitbox.y * 0.5f - collision.centre.y + eps;
				velocity.SetZero();
			}
			else if (maxPlayer.y <= minCube.y && minPlayer.y <= minCube.y) //bump head
			{
				//position.y = minCube.y - Player.collision.hitbox.y * 0.5f + collision.centre.y;
				velocity.SetZero();
			}
		}
	}
}
