#include "Entity.h"

Entity::Entity() : position(0,0,0), velocity(0,0,0), lifetime(0.f), hOrientation(0.f), vOrientation(0), size(1,1,1), mesh(NULL), showOnMinimap(false), jump(false), sneak(false)
{
	active = true;
}

Entity::~Entity()
{
}

void Entity::Update(double dt)
{
}

void Entity::RenderObject(MS& modelStack)
{
	modelStack.Translate(position);
	modelStack.Rotate(hOrientation, 0, 1, 0);
	modelStack.Scale(size);
}

bool Entity::IsActive()
{
	return active;
}
void Entity::SetActive(bool active)
{
	this->active = active;
}

#define eps 0.000001f;

void Entity::Update(double dt, const vector<Block*>& object, bool RestrictMovement)
{
}

void Entity::RespondToCollision(const vector<Block*>&object)
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

	bool stepped = false;
	bool shouldClimb = false;
	float climb = 0.f;
	bool canClimb = true;

	count = collidedBlocks.size();
	for (unsigned i = 0; i < count; ++i)
	{
		Block P(position, collision.centre, collision.hitbox);
		if (Block::checkCollision(P, collidedBlocks[i]))
		{
			Vector3 maxPlayer = initialPos + Player.collision.centre + Player.collision.hitbox  * 0.5f;
			Vector3 minPlayer = initialPos + Player.collision.centre - Player.collision.hitbox * 0.5f;

			Vector3 maxCube = collidedBlocks[i].getMaxCoord();
			Vector3 minCube = collidedBlocks[i].getMinCoord();

			if (maxPlayer.z >= maxCube.z && minPlayer.z >= maxCube.z)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - 0.5f && minPlayer.y >= maxCube.y - 0.5f)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = true;
						climb = maxCube.y + eps;
					}
				}
				else
				{
					canClimb = false;
					velocity.z = 0;
					position.z = maxCube.z + Player.collision.hitbox.z * 0.5f + eps;
				}
			}
			else if (maxPlayer.z <= minCube.z && minPlayer.z <= minCube.z)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - 0.5f && minPlayer.y >= maxCube.y - 0.5f)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = true;
						climb = maxCube.y + eps;
					}
				}
				else
				{
					canClimb = false;
					velocity.z = 0;
					position.z = minCube.z - Player.collision.hitbox.z * 0.5f - eps;
				}
			}
			else if (maxPlayer.x >= maxCube.x && minPlayer.x >= maxCube.x)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - 0.5f && minPlayer.y >= maxCube.y - 0.5f)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = true;
						climb = maxCube.y + eps;
					}
				}
				else
				{
					canClimb = false;
					velocity.x = 0;
					position.x = maxCube.x + Player.collision.hitbox.x * 0.5f + eps;
				}
			}
			else if (maxPlayer.x <= minCube.x && minPlayer.x <= minCube.x)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - 0.5f && minPlayer.y >= maxCube.y - 0.5f)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = true;
						climb = maxCube.y + eps;
					}
				}
				else
				{
					canClimb = false;
					velocity.x = 0;
					position.x = minCube.x - Player.collision.hitbox.x * 0.5f - eps;
				}
			}
			else if (maxPlayer.y >= maxCube.y && minPlayer.y >= maxCube.y)
			{
				position.y = maxCube.y + eps;
				velocity.y = 0;

				if (sneak)
				{
					if (position.z - Player.collision.hitbox.z * 0.4f > maxCube.z)
						position.z = maxCube.z + Player.collision.hitbox.z * 0.4f;
					else if (position.z + Player.collision.hitbox.z * 0.4f < minCube.z)
						position.z = minCube.z - Player.collision.hitbox.z * 0.4f;
					if (position.x - Player.collision.hitbox.x * 0.4f > maxCube.x)
						position.x = maxCube.x + Player.collision.hitbox.x * 0.4f;
					else if (position.x + Player.collision.hitbox.x * 0.4f < minCube.x)
						position.x = minCube.x - Player.collision.hitbox.x * 0.4f;
				}
				else if (!stepped)
				{
					stepSound = Block::getStepSound(collidedBlocks[i].id);
					stepped = true;
				}
			}
			else if (maxPlayer.y <= minCube.y && minPlayer.y <= minCube.y) //bump head
			{
				position.y = minCube.y - Player.collision.hitbox.y;
				velocity.y = -0.1f;
			}
		}
	}

	if (canClimb && shouldClimb)
	{
		position.y = climb;
	}

	collidedBlocks.clear();
	penetrationArea.clear();

}