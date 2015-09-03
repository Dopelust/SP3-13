/******************************************************************************/
/*!
\file	player.cpp
\author Ricsson
\par
\brief
This is the camera3 cpp
*/
/******************************************************************************/
#include <iomanip>

#include "player.h"
#include "MinScene.h"

using namespace::std;

/******************************************************************************/
/*!
\brief CollisionResponse function
*/
/******************************************************************************/
extern ISoundEngine * engine;

void Player::Init()
{
	position.Set(0, 0, 0);
	velocity.SetZero();
	kbVelocity.SetZero();
	hOrientation = 0;
	vOrientation = 0;
	stepSound = NULL;
	mount = NULL;
	eyeLevel = 1.62f;
	collision.hitbox.Set(0.6f, 1.8f, 0.6f);
	collision.centre.Set(0, collision.hitbox.y / 2, 0);
	maxHealth = health = 100;
	camera.Init(Vector3(position.x, position.y + eyeLevel, position.z), hOrientation, vOrientation);
	SprintBar = MaxSprintTime = 4;
	SprintCounter = ((SprintBar/MaxSprintTime) * 100);
	Sprint = true;
	run = false;
	Horse = 1;

	/******************/
    // Skill Tree
	skillPoint = 0;
	damageCounter = 1;
	speedCounter = 2;
	addHealth = false;
	addStamina = false;
	addDamage = false;
	addSpeed = false;
	maxEXP = 20;
	currentEXP = 0;
	Lv = 1;

	for (unsigned i = 0; i < 255; ++i)
	{
		myKeys[i] = false;
	}
}
void Player::UpdateVelocity(double dt)
{
	if (jump && velocity.y == 0)
		jump = false;

	velocity.y -= 30.f * (float)dt;
}

void Player::Update(double dt, bool RestrictMovement)
{
	if (mount && !mount->IsActive())
		mount = NULL;
	
	sneak = false;
	initialVel = velocity;
	initialPos = position;
	Vector3 direction; Vector3 right;
	direction.SphericalToCartesian(hOrientation, 0.0f);
	right.SphericalToCartesian(hOrientation - 90.f, 0.0f);

	if (noClip)
	{
		WALK_SPEED = 8;
		if (myKeys['C'])
			WALK_SPEED *= 4;

		velocity.SetZero();

		Vector3 hVel;
		if (myKeys['w'])
			hVel += direction;
		if (myKeys['a'])
			hVel += -right;
		if (myKeys['s'])
			hVel += -direction;
		if (myKeys['d'])
			hVel += right;

		if (hVel.LengthSquared() > 0)
		{
			hVel.Normalize() *= WALK_SPEED;
			velocity.x = hVel.x; velocity.z = hVel.z;
		}
		else
		{
			velocity.x += -velocity.x * dt * 16;
			velocity.z += -velocity.z * dt * 16;
			if (velocity.x > -0.1f && velocity.x < 0.1f)
				velocity.x = 0;
			if (velocity.z > -0.1f && velocity.z < 0.1f)
				velocity.z = 0;
		}

		if (myKeys[' '])
			velocity.y += WALK_SPEED;
		if (myKeys['S'])
			velocity.y -= WALK_SPEED;

		position += velocity * dt;
		if (velocity.y > 0)
			Fall(velocity.y, dt * 16, 0);
		else if (velocity.y < 0)
			Rise(velocity.y, dt * 16, 0);

		Vector3 camerapos = position; camerapos.y += eyeLevel;
		camera.position += (camerapos - camera.position) * 25 * dt;
		if (!RestrictMovement) camera.Move(dt);
		camera.Update();
		hOrientation = camera.orientation;
		vOrientation = camera.look;

		for (unsigned i = 0; i < 255; ++i)
		{
			myKeys[i] = false;

		}
		return;
	}

	if (myKeys['S'])
	{
		sneak = true;
		WALK_SPEED = 1.295f;
		eyeLevel = 1.52f;
		Fall(camera.fov, 100 * dt, 70);
		Rise(SprintBar, dt, MaxSprintTime);
	}
	else
	{
		if (myKeys['C'] && myKeys['w'] && Sprint && run && !mount)
		{
			SprintBar -= dt;
			WALK_SPEED = 5.612f;
			Rise(camera.fov, 200 * dt, 85);
		}
		else
		{
			WALK_SPEED = 4.317f;
			Fall(camera.fov, 100 * dt, 70);
			Rise(SprintBar, dt, MaxSprintTime);
		}

		eyeLevel = 1.62f;
	}

	if (SprintBar < 0)
	{
		ISound* sound = engine->play2D("Assets/Media/Panting.mp3", false, true);
		if (sound)
		{
			sound->setVolume(0.2f);
			sound->setIsPaused(false);
		}
		Sprint = false;
	}
	else if (SprintBar == MaxSprintTime)
	{
		Sprint = true;
	}

	Vector3 hVel;
	if (myKeys['w'])
		hVel += direction;
	if (myKeys['a'])
		hVel += -right;
	if (myKeys['s'])
		hVel += -direction;
	if (myKeys['d'])
		hVel += right;

	if (hVel.LengthSquared() > 0)
	{
		run = true;
		hVel.Normalize() *= WALK_SPEED;
		velocity.x = hVel.x; velocity.z = hVel.z;
	}
	else
	{
		run = false;
		velocity.x += -velocity.x * dt * 16;
		velocity.z += -velocity.z * dt * 16;
		if (velocity.x > -0.1f && velocity.x < 0.1f)
			velocity.x = 0;
		if (velocity.z > -0.1f && velocity.z < 0.1f)
			velocity.z = 0;
	}

	kbVelocity += -kbVelocity * 16 * dt;
	kbVelocity.x = kbVelocity.x > -0.1f && kbVelocity.x < 0.1f ? 0 : kbVelocity.x;
	kbVelocity.z = kbVelocity.z > -0.1f && kbVelocity.z < 0.1f ? 0 : kbVelocity.z;

	UpdateVelocity(dt);
	if (!mount)
		if (myKeys[' '] && !jump && (velocity.y > -2.f && velocity.y <= 0))
		{
			jump = true;
			velocity.y = 8.25f;
		}

	position += velocity * dt;
	position += kbVelocity * dt;

	RespondToCollision(collisionBlockList);

	if (mount)
	{
		WALK_SPEED = 11;
		hVel.SetZero();

		if (myKeys['w'])
			hVel += direction * WALK_SPEED;
		if (myKeys['a'])
			hVel += -right * WALK_SPEED * 0.25f;
		if (myKeys['s'])
			hVel += -direction * WALK_SPEED * 0.25f;
		if (myKeys['d'])
			hVel += right * WALK_SPEED * 0.25f;

		if (hVel.LengthSquared() > 0)
		{
			mount->velocity.x = hVel.x; mount->velocity.z = hVel.z;
		}

		if (mount->jump && mount->velocity.y == 0)
			mount->jump = false;
		if (myKeys[' '] && !mount->jump && (mount->velocity.y > -2.f && mount->velocity.y <= 0))
		{
			mount->jump = true;
			mount->velocity.y = 14.f;

			char* soundFileName[3] = { "Assets/Media/Horse/horseJump1.mp3", "Assets/Media/Horse/horseJump2.mp3", "Assets/Media/Horse/horseJump3.mp3" };

			ISound* sound = engine->play2D(soundFileName[rand() % 3], false, true);
			if (sound)
			{
				sound->setVolume(0.4f);
				sound->setIsPaused(false);
			}
		}
		mount->hOrientation = hOrientation;

		position = mount->position;
		position.y += 1.1f;
		velocity.SetZero();

		if (!mount->jump)
		{
			stepRate += dt;

			if (!hVel.IsZero())
			{
				float bobSpeed = 0.5f;
				if (!myKeys['w'])
					bobSpeed = 0.25f;

				if (stepRate >= 0.4f)
				{
					if (myKeys['w'])
					{
						char* soundFileName[4] = { "Assets/Media/Horse/Horse1.mp3", "Assets/Media/Horse/Horse2.mp3", "Assets/Media/Horse/Horse3.mp3", "Assets/Media/Horse/Horse4.mp3" };

						ISound* sound = engine->play2D(soundFileName[rand() % 4], false, true);
						if (sound)
						{
							sound->setVolume(0.2f);
							sound->setIsPaused(false);
						}
					}
					else
					{
						char* soundFileName[6] = { "Assets/Media/Horse/horseSide1.mp3", "Assets/Media/Horse/horseSide2.mp3", "Assets/Media/Horse/horseSide3.mp3", "Assets/Media/Horse/horseSide4.mp3", "Assets/Media/Horse/horseSide5.mp3", "Assets/Media/Horse/horseSide6.mp3" };

						ISound* sound = engine->play2D(soundFileName[rand() % 6], false, true);
						if (sound)
						{
							sound->setVolume(0.5f);
							sound->setIsPaused(false);
						}
					}
					stepRate = 0;
				}

				if (Horse > 1.05f)
					Riding = true;
				else if (Horse < 0.95f)
					Riding = false;

				if (Riding)
					Horse -= dt*bobSpeed;
				else
					Horse += dt*bobSpeed;
				mount->size.y = Horse;
			}
		}
	}
	else
	{
		hVel.Set(velocity.x, 0, velocity.z);

		if (!hVel.IsZero())
		{
			if (myKeys['S'])
				Steps += dt * 0.5f;
			else if (myKeys['C'])
				Steps += dt * 2;
			else
				Steps += dt;

			if (getSelectedItem())
				getSelectedItem()->Bob(Steps);
		}

		if (velocity.y == 0)
		{
			if (!sneak && run)
			{
				stepRate += dt;

				if (myKeys['C'])
					stepRate += dt;
			}

			if (stepRate >= 0.5f)
			{
				hVel.Set(velocity.x, 0, velocity.z);
				if (hVel.LengthSquared() > 0.25f*0.25f)
				{
					stepRate = 0.f;

					if (stepSound)
					{
						ISound * sound = engine->play2D(stepSound, false, true);
						if (sound)
						{
							sound->setVolume(0.25f);
							sound->setIsPaused(false);
						}
					}
				}
			}

			if (initialVel.y < -5)
			{
				if (initialVel.y < -14)
				{
					char* soundFileName[3];
					soundFileName[0] = "Assets/Media/Damage/hit1.mp3";
					soundFileName[1] = "Assets/Media/Damage/hit2.mp3";
					soundFileName[2] = "Assets/Media/Damage/hit3.mp3";

					ISound* sound = engine->play2D(soundFileName[rand() % 3], false, true);
					if (sound)
					{
						sound->setVolume(0.75f);
						sound->setIsPaused(false);
					}
					reduceHealth(-initialVel.y);
				}
				else if(stepSound)
				{
					ISound * sound = engine->play2D(stepSound, false, true);
					if (sound)
					{
						sound->setVolume(0.25f);
						sound->setIsPaused(false);
					}
				}
			}
		}
	}

	Vector3 camerapos = position; camerapos.y += eyeLevel;
	Vector3 initialDir = camerapos - camera.position;
	camera.position +=  initialDir * 25 * dt;
	Vector3 finalDir = camerapos - camera.position;

	if (!RestrictMovement) camera.Move(dt);
	camera.Update();
	hOrientation = camera.orientation;
	vOrientation = camera.look;

	for (unsigned i = 0; i < 255; ++i)
	{
		myKeys[i] = false;
	}

	SprintCounter = ((SprintBar/MaxSprintTime) * 100);
	WorldBorderCheck();
}

void Player::reduceHealth(float reduction)
{
	health -= reduction;

	if (health < 1)
		health = 0;
}
CItem * Player::getSelectedItem()
{
	return inventory.selectedSlot->item;
}
void Player::recoverHealth(float recovery)
{
	health += recovery;

	if (health > maxHealth)
		health = maxHealth;
}

void Player::updateEXP(double dt)
{
	currentEXP += 10;
	if (currentEXP > maxEXP - 1)
	{
		//Get Skill points  
		skillPoint++;
		maxEXP += 20;
		currentEXP = 0;
		Lv++;
	}
}

void Player::UpdateSkilltree(double dt)
{
	if (skillPoint > 0)
	{
		if (addHealth)
		{
			maxHealth += 10;
			skillPoint--;
			addHealth = false;
		}

		if (addSpeed)
		{
			speedCounter += 0.1f;

			CKnife* K = dynamic_cast<CKnife*>(inventory.slot[0]->item);
			K->attackRate = 1.f / speedCounter;

			skillPoint--;
			addSpeed = false;
		}

		if (addStamina)
		{
			MaxSprintTime++;
			skillPoint--;
			addStamina = false;
		}

		if (addDamage)
		{
			damageCounter += 0.2f;
			skillPoint--;
			addDamage = false;
		}
	}

}