#include "Entity.h"
#include "MinScene.h"

#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib")

using namespace irrklang;

extern ISoundEngine * engine;

Entity::Entity() : position(0,0,0), velocity(0,0,0), lifetime(0.f), headOrientation(0.f), hOrientation(0.f), vOrientation(0), size(1,1,1), mesh(NULL), showOnMinimap(false), jump(false), sneak(false), climbHeight(0.5f), dead(false), Steps(0.f), stepRate(0.f), health(100), aggro(NULL), mount(NULL), viewRange(0), maxViewRange(0)
{
	active = true;
}

Entity::~Entity()
{
}

void Entity::Update(double dt)
{
}

Key::Key()
{
}

Key::~Key()
{
}

void Key::Update(double dt, bool RestrictMovement)
{
	hOrientation += dt * 30;
}

void Entity::RenderObject(MS& modelStack)
{
	modelStack.Translate(position);
	modelStack.Rotate(hOrientation, 0, 1, 0);
	modelStack.Scale(size);
}

void Entity::Knockback(Vector3 dir)
{
	kbVelocity.x += dir.x;
	if (velocity.y <= 0)
		velocity.y += 5;
	kbVelocity.z += dir.z;

	vector<char*> soundFileName;

	if (entityID == HORSE)
	{
		soundFileName.push_back("Assets/Media/Damage/horseCry1.mp3");
		soundFileName.push_back("Assets/Media/Damage/horseCry2.mp3");
		soundFileName.push_back("Assets/Media/Damage/horseCry3.mp3");
		soundFileName.push_back("Assets/Media/Damage/horseCry4.mp3");
	}
	//else if (entityID == SENTRY || entityID == ENEMY_2 || entityID == ENEMY_3)
	//{
	//	// Enemy Kenna Hit Sound
	//	soundFileName.push_back("Assets/Media/Damage/enemyCry1.mp3");
	//	soundFileName.push_back("Assets/Media/Damage/enemyCry2.mp3");
	//	soundFileName.push_back("Assets/Media/Damage/enemyCry3.mp3");
	//}
	else if (entityID == WOLF)
	{
		// Wolf Kenna Hit Sound
		soundFileName.push_back("Assets/Media/Damage/wolfCry1.mp3");
		soundFileName.push_back("Assets/Media/Damage/wolfCry2.mp3");
		soundFileName.push_back("Assets/Media/Damage/wolfCry3.mp3");
	}
	else
	{
		soundFileName.push_back("Assets/Media/Damage/hit1.mp3");
		soundFileName.push_back("Assets/Media/Damage/hit2.mp3");
		soundFileName.push_back("Assets/Media/Damage/hit3.mp3");
	}

	ISound* sound = engine->play3D(soundFileName[rand() % soundFileName.size()], vec3df(position.x, position.y, position.z), false, true);
	if (sound)
	{
		sound->setVolume(1.f);
		sound->setIsPaused(false);
	}

	health -= dir.LengthSquared() * 0.01f;
}

bool Entity::IsActive()
{
	return active;
}
void Entity::SetActive(bool active)
{
	this->active = active;
}

void Entity::ClearArrows()
{
	while (!StuckedArrows.empty())
	{
		delete StuckedArrows.back();
		StuckedArrows.pop_back();
	}
}

void Entity::WorldBorderCheck()
{
	position.x = position.x > (float)worldX*0.5f ? (float)worldX * 0.5f : position.x;
	position.x = position.x < (float)worldX*-0.5f ? (float)worldX * -0.5f : position.x;
	position.z = position.z > (float)worldZ*0.5f ? (float)worldZ * 0.5f : position.z;
	position.z = position.z < (float)worldZ*-0.5f ? (float)worldZ * -0.5f : position.z;

	if (position.y < -9)
	{
		position.y = -9;
		velocity.y = 0;
	}
}
bool Entity::IsLiving()
{
	return false;
}

bool Entity::IsDead()
{
	return dead;
}
void Entity::SetDead(bool dead)
{
	this->dead = dead;
}

bool Entity::canAttack()
{
	return false;
}
void Entity::Attack()
{
}

#define eps 0.000001f;

float Entity::getSkeletalRotation()
{
	return sin(Math::DegreeToRadian(Steps)) * 30;
}

void Entity::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	velocity.x += -velocity.x * 10 * dt;
	velocity.z += -velocity.z * 10 * dt;
	kbVelocity += -kbVelocity * 16 * dt;

	velocity.x = velocity.x > -0.1f && velocity.x < 0.1f ? 0 : velocity.x;
	velocity.z = velocity.z > -0.1f && velocity.z < 0.1f ? 0 : velocity.z;
	kbVelocity.x = kbVelocity.x > -0.1f && kbVelocity.x < 0.1f ? 0 : kbVelocity.x;
	kbVelocity.z = kbVelocity.z > -0.1f && kbVelocity.z < 0.1f ? 0 : kbVelocity.z;

	velocity.y -= 30 * dt;

	position += velocity * dt;
	position += kbVelocity * dt;

	RespondToCollision(this->collisionBlockList);
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
	int shouldClimb = 0;
	vector<int> cantClimb;
	Vector3 failToClimb;
	float climb = 0.f;

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
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - climbHeight && minPlayer.y >= maxCube.y - climbHeight)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = 1;
						climb = maxCube.y + eps;
						failToClimb.z = maxCube.z + Player.collision.hitbox.z * 0.5f + eps;
					}
				}
				else
				{
					cantClimb.push_back(1);
					kbVelocity.z = velocity.z = 0;
					position.z = maxCube.z + Player.collision.hitbox.z * 0.5f + eps;
				}
			}
			else if (maxPlayer.z <= minCube.z && minPlayer.z <= minCube.z)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - climbHeight && minPlayer.y >= maxCube.y - climbHeight)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = 2;
						climb = maxCube.y + eps;
						failToClimb.z = minCube.z - Player.collision.hitbox.z * 0.5f - eps;
					}
				}
				else
				{
					cantClimb.push_back(2);
					kbVelocity.z = velocity.z = 0;
					position.z = minCube.z - Player.collision.hitbox.z * 0.5f - eps;
				}
			}
			else if (maxPlayer.x >= maxCube.x && minPlayer.x >= maxCube.x)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - climbHeight && minPlayer.y >= maxCube.y - climbHeight)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = 3;
						climb = maxCube.y + eps;
						failToClimb.x = maxCube.x + Player.collision.hitbox.x * 0.5f + eps;
					}
				}
				else
				{
					cantClimb.push_back(3);
					kbVelocity.x = velocity.x = 0;
					position.x = maxCube.x + Player.collision.hitbox.x * 0.5f + eps;
				}
			}
			else if (maxPlayer.x <= minCube.x && minPlayer.x <= minCube.x)
			{
				if (velocity.y > -1.f && !jump && maxPlayer.y >= maxCube.y - climbHeight && minPlayer.y >= maxCube.y - climbHeight)
				{
					if (position.y < maxCube.y)
					{
						shouldClimb = 4;
						climb = maxCube.y + eps;
						failToClimb.x = minCube.x - Player.collision.hitbox.x * 0.5f - eps;
					}
				}
				else
				{
					cantClimb.push_back(4);
					kbVelocity.x = velocity.x = 0;
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
				if (!stepped)
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

	if (shouldClimb)
	{
		bool canClimb = true;

		for (unsigned i = 0; i < cantClimb.size(); ++i)
		{
			if (shouldClimb == cantClimb[i])
				canClimb = false;
		}
		if (canClimb)
		{
			Block P(Vector3(position.x, climb, position.z), collision.centre, collision.hitbox);

			unsigned count = object.size();
			for (unsigned i = 0; i < count; ++i)
			{
				if (object[i]->position != shouldClimb)
				{
					if (object[i]->type == Block::STAIR)
					{
						for (unsigned j = 0; j < object[i]->collisions.size(); ++j)
						{
							Block Blk(object[i]->position, object[i]->collisions[j].centre, object[i]->collisions[j].hitbox);
							Blk.id = object[i]->id;

							if (Block::checkCollision(P, Blk))
							{
								if (failToClimb.x != 0)
									position.x = failToClimb.x;
								if (failToClimb.z != 0)
									position.z = failToClimb.z;
								return;
							}
						}
					}
					else
					{
						if (Block::checkCollision(P, *object[i]))
						{
							if (failToClimb.x != 0)
								position.x = failToClimb.x;
							if (failToClimb.z != 0)
								position.z = failToClimb.z;
							return;
						}
					}
				}
			}
			
			position.y = climb;
		}
	}
}

string Entity::getSpeech()
{
	return NULL;
}

unsigned Entity::getSubID()
{
	return 0;
}