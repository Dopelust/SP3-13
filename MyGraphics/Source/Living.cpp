#include "Living.h"
#include "Utility.h"

Living::Living() : timeToStop(0), timeToGo(0)
{
	timeToGo = rand() % 10 + 1;
}

Living::~Living()
{
}

void Living::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	if (timeToGo > 0)
	{
		if (newOrientation > hOrientation)
			Rise(hOrientation, dt * 25, newOrientation);
		else if (newOrientation < hOrientation)
			Fall(hOrientation, dt * 25, newOrientation);

		Vector3 dir; dir.SphericalToCartesian(hOrientation, 0);
		velocity.x = dir.x; velocity.z = dir.z;

		Fall(timeToGo, dt, 0);

		if (timeToGo == 0)
			timeToStop = rand() % 8 + 3;
	}
	else if (timeToStop > 0)
	{
		Fall(timeToStop, dt, 0);

		if (timeToStop == 0)
		{
			timeToGo = rand() % 10 + 1;
			newOrientation = hOrientation + (rand() % 181 - 90);
		}
	}

	position += velocity * dt;

	velocity.x += -velocity.x * 16 * dt;
	velocity.z += -velocity.z * 16 * dt;
	if (velocity.x > -0.1f && velocity.x < 0.1f)
		velocity.x = 0;
	if (velocity.z > -0.1f && velocity.z < 0.1f)
		velocity.z = 0;
	velocity.y -= 30 * dt;

	RespondToCollision(this->collisionBlockList);

	if (timeToGo)
	if ((initialPos.x == position.x || initialPos.z == position.z) && velocity.y == 0 && !velocity.IsZero())
		velocity.y = 7;

	Animate(dt);
}

void Living::Animate(double dt)
{
	if (!Vector3(velocity.x, 0, velocity.z).IsZero())
	{
		Steps += dt * 300;
	}
	else if ((int)Steps % 180 != 0)
	{
		int valueToRise = Steps;
		while (valueToRise % 180 != 0)
		{
			valueToRise++;
		}

		Rise(Steps, dt * 300, valueToRise);
	}

	if (timeToStop > 0)
	{
		headBob += dt * 100;
	}
	else if ((int)headBob % 180 != 0)
	{
		int valueToRise = headBob;
		while (valueToRise % 180 != 0)
		{
			valueToRise++;
		}

		Rise(headBob, dt * 100, valueToRise);
	}
	headOrientation = sin(Math::DegreeToRadian(headBob)) * 20;
}

Arrow::Arrow() : relativePosition(0,0,0), relativeOrientation(0)
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

Drop::Drop()
{
}

Drop::~Drop()
{
}

void Drop::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	position += velocity * dt;
	
	velocity.x += -velocity.x * 2 * dt;
	velocity.z += -velocity.z * 2 * dt;
	velocity.y -= 30 * dt;
	if (velocity.x > -0.1f && velocity.x < 0.1f)
		velocity.x = 0;
	if (velocity.z > -0.1f && velocity.z < 0.1f)
		velocity.z = 0;

	hOrientation += Vector3(velocity.x, 0, velocity.z).LengthSquared() * dt * 300;

	RespondToCollision(this->collisionBlockList);
}

void Drop::RespondToCollision(const vector<Block*>&object)
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
				velocity.z = -velocity.z * 0.5f;
				position.z = maxCube.z + Player.collision.hitbox.z * 0.5f + eps;
			}
			else if (maxPlayer.z <= minCube.z && minPlayer.z <= minCube.z)
			{
				velocity.z = -velocity.z * 0.5f;
				position.z = minCube.z - Player.collision.hitbox.z * 0.5f - eps;
			}
			else if (maxPlayer.x >= maxCube.x && minPlayer.x >= maxCube.x)
			{
				velocity.x = -velocity.x * 0.5f;
				position.x = maxCube.x + Player.collision.hitbox.x * 0.5f + eps;
			
			}
			else if (maxPlayer.x <= minCube.x && minPlayer.x <= minCube.x)
			{
				velocity.x = -velocity.x * 0.5f;
				position.x = minCube.x - Player.collision.hitbox.x * 0.5f - eps;
			}
			else if (maxPlayer.y >= maxCube.y && minPlayer.y >= maxCube.y)
			{
				position.y = maxCube.y + eps;
				velocity.y = -velocity.y * 0.5f;

				if (velocity.y > -1.f && velocity.y < 1.f)
					velocity.y = 0;
			}
			else if (maxPlayer.y <= minCube.y && minPlayer.y <= minCube.y) //bump head
			{
				position.y = minCube.y - Player.collision.hitbox.y;
				velocity.y = -velocity.y;
			}
		}
	}
}

Enemy::Enemy()
{
	state = 1;
}

Enemy::Enemy(int state, double patrolTime, double stopToCheckTime)
{
	this->state = state;
	if (this->state == 1)
	{
		MoveTo = true;
		MoveBack = false;
		turn = false;
	}

	if (this->state == 2)
	{
		this->patrolTime = patrolTime;
		this->stopToCheckTime = stopToCheckTime;
		MoveTo1 = true;
		MoveTo2 = false;
		MoveTo3 = false;
		MoveTo4 = false;
		stopToCheck = false;
		stopTimer = 0;
	}
}

Enemy::~Enemy()
{
}

void Enemy::Update(double dt, bool RestrictMovement)
{
	initialPos = position;
	initialVel = velocity;

	Vector3 dir; dir.SphericalToCartesian(hOrientation, 0);
	velocity.x = dir.x; velocity.z = dir.z;

	if (this->state == 1)
		Idle(dt);
	else if (this->state == 2)
		Patrol(dt);
	//else if (this->state == 3)
		//Chase(dt);

	if (velocity.x > -0.1f && velocity.x < 0.1f)
		velocity.x = 0;
	if (velocity.z > -0.1f && velocity.z < 0.1f)
		velocity.z = 0;
	velocity.y -= 30 * dt;

	RespondToCollision(this->collisionBlockList);

	if ((initialPos.x == position.x || initialPos.z == position.z) && velocity.y == 0 && !velocity.IsZero())
		velocity.y = 8.25f;

	Animate(dt);
}

void Enemy::Idle(double dt)
{
	stepCounter += dt;

	if (MoveTo == true && MoveBack == false)	//Move to a place 5 seconds away
		position += velocity * dt;

	if (MoveTo == false && MoveBack == true)	//Move back to original place (5 seconds away)
		position += velocity * dt;

	velocity.x += -velocity.x * dt * 16;
	velocity.z += -velocity.z * dt * 16;

	if (turn == true)		//If enemy is supposed to turn
	{
		hOrientation += 180.f;
		turn = false;
	}

	if (stepCounter > 5.f && MoveTo == true)  //If enemy hits new position
	{
		MoveTo = false;
		MoveBack = true;
		turn = true;
		stepCounter = 0;
	}

	if (stepCounter > 5.f && MoveBack == true) //If enemy returns to old position
	{
		MoveTo = true;
		MoveBack = false;
		turn = true;
		stepCounter = 0;

	}
}

void Enemy::Patrol(double dt)
{
	stepCounter += dt;

	PatrolSquare(dt);
	//PatrolTriangle(dt);

}

void Enemy::PatrolSquare(double dt)
{
	//cout << stopTimer << endl;

	if (MoveTo1 == true && MoveTo2 == false && MoveTo3 == false && MoveTo4 == false && stopToCheck == false)
		position += velocity * dt;
	else if (MoveTo1 == false && MoveTo2 == true && MoveTo3 == false && MoveTo4 == false && stopToCheck == false)
		position += velocity * dt;
	else if (MoveTo1 == false && MoveTo2 == false && MoveTo3 == true && MoveTo4 == false && stopToCheck == false)
		position += velocity * dt;
	else if (MoveTo1 == false && MoveTo2 == false && MoveTo3 == false && MoveTo4 == true && stopToCheck == false)
		position += velocity * dt;
	else
	{
		velocity.x = 0; velocity.z = 0;
		position += velocity * dt;
	}

	velocity.x += -velocity.x * dt * 16;
	velocity.z += -velocity.z * dt * 16;

	if (turn == true)		//If enemy is supposed to turn
	{
		hOrientation += 90.f;
		turn = false;
	}

	if (stepCounter > patrolTime && MoveTo1 == true)  //If enemy hits 1st new position
	{

		stopToCheck = true;
		if (stopToCheck == true)
		{
			stepCounter += 0;
			stopTimer += dt;
			if (stopTimer > stopToCheckTime)
			{
				MoveTo1 = false;
				MoveTo2 = true;
				turn = true;
				stepCounter = 0;
				stopTimer = 0;
				stopToCheck = false;
			}
		}

	}

	if (stepCounter > patrolTime && MoveTo2 == true)  //If enemy hits 2nd new position
	{
		stopToCheck = true;
		if (stopToCheck == true)
		{
			stepCounter += 0;
			stopTimer += dt;
			if (stopTimer > stopToCheckTime)
			{
				MoveTo2 = false;
				MoveTo3 = true;
				turn = true;
				stepCounter = 0;
				stopTimer = 0;
				stopToCheck = false;
			}
		}
	}

	if (stepCounter > patrolTime && MoveTo3 == true)  //If enemy hits 3rd new position
	{
		stopToCheck = true;
		if (stopToCheck == true)
		{
			stepCounter += 0;
			stopTimer += dt;
			if (stopTimer > stopToCheckTime)
			{
				MoveTo3 = false;
				MoveTo4 = true;
				turn = true;
				stepCounter = 0;
				stopTimer = 0;
				stopToCheck = false;
			}
		}
	}

	if (stepCounter > patrolTime && MoveTo4 == true)  //If enemy hits 4th new position
	{
		stopToCheck = true;
		if (stopToCheck == true)
		{
			stepCounter += 0;
			stopTimer += dt;
			if (stopTimer > stopToCheckTime)
			{
				MoveTo4 = false;
				MoveTo1 = true;
				turn = true;
				stepCounter = 0;
				stopTimer = 0;
				stopToCheck = false;
			}
		}
	}
}

void Enemy::PatrolTriangle(double dt)
{
	if (MoveTo1 == true && MoveTo2 == false && MoveTo3 == false && MoveTo4 == false)
		position += velocity * dt;
	if (MoveTo1 == false && MoveTo2 == true && MoveTo3 == false && MoveTo4 == false)
		position += velocity * dt;
	if (MoveTo1 == false && MoveTo2 == false && MoveTo3 == true && MoveTo4 == false)
		position += velocity * dt;

	velocity.x += -velocity.x * dt * 16;
	velocity.z += -velocity.z * dt * 16;

	if (turn1 == true)		//If enemy is supposed to turn
	{
		hOrientation += 90.f;
		turn1 = false;
	}

	if (turn2 == true)
	{
		hOrientation += 120.f;
		turn2 = false;
	}

	if (stepCounter > patrolTime && MoveTo1 == true)  //If enemy hits 1st new position
	{
		MoveTo1 = false;
		MoveTo2 = true;
		turn1 = true;
		stepCounter = 0;
	}

	if (stepCounter > patrolTime && MoveTo2 == true)  //If enemy hits 2nd new position
	{
		MoveTo2 = false;
		MoveTo3 = true;
		turn2 = true;
		stepCounter = 0;
	}

	if (stepCounter > patrolTime && MoveTo3 == true)  //If enemy hits 3rd new position
	{
		MoveTo3 = false;
		MoveTo1 = true;
		turn2 = true;
		stepCounter = 0;
	}
}