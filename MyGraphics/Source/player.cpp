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
	hOrientation = 0;
	vOrientation = 0;
	stepSound = NULL;
	eyeLevel = 1.62f;
	collision.hitbox.Set(0.6f, 1.8f, 0.6f);
	collision.centre.Set(0, collision.hitbox.y / 2, 0);
	stepRate = 0;
	apparentHealth = trueHealth = 100;
	camera.Init(Vector3(position.x, position.y + eyeLevel, position.z), hOrientation, vOrientation);

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

void Player::Update(double dt, const vector<Block*>&object, bool RestrictMovement)
{
	sneak = false;
	initialVel = velocity;
	initialPos = position;
	Vector3 direction; Vector3 right;
	direction.SphericalToCartesian(hOrientation, 0.0f);
	right.SphericalToCartesian(hOrientation - 90.f, 0.0f);

	if (noClip)
	{
		WALK_SPEED = 8;
		velocity.SetZero();

		Vector3 hVel;
		if (myKeys['w'])
			hVel += direction * WALK_SPEED;
		if (myKeys['a'])
			hVel += -right * WALK_SPEED;
		if (myKeys['s'])
			hVel += -direction * WALK_SPEED;
		if (myKeys['d'])
			hVel += right * WALK_SPEED;

		if (hVel.LengthSquared() > 1)
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
	}
	else if (myKeys['C'])
	{
		WALK_SPEED = 5.612f;
		eyeLevel = 1.62f;
	}
	else
	{
		WALK_SPEED = 4.317f;
		eyeLevel = 1.62f;
	}

	Vector3 hVel;
	if(myKeys['w'])
		hVel += direction * WALK_SPEED;
	if(myKeys['a'])
		hVel += -right * WALK_SPEED;
	if(myKeys['s'])
		hVel += -direction * WALK_SPEED;
	if(myKeys['d'])
		hVel += right * WALK_SPEED;

	if (hVel.LengthSquared() > 1)
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

	UpdateVelocity(dt);
	if(myKeys[' '] && !jump && (velocity.y > -2.f && velocity.y <= 0))
	{
		jump = true;
		velocity.y = 8.25f;
	}

	position += velocity * dt;
	RespondToCollision(object);
		
	if (velocity.y == 0)
	{	
		if (!myKeys['S'])
		{
			float step = 0.f;

			if (myKeys['C'])
				step = 0.3f;
			else
				step = 0.45f;

			if (stepRate >= step)
			{
				hVel.Set(velocity.x, 0, velocity.z);
				if( hVel.LengthSquared() > 0.25f*0.25f)
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
		}
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
			reduceHealth(-initialVel.y/3);
		}
	}

	Vector3 camerapos = position; camerapos.y += eyeLevel;
	camera.position += (camerapos - camera.position) * 25 * dt;
	if (!RestrictMovement) camera.Move(dt);
	camera.Update();
	hOrientation = camera.orientation;
	vOrientation = camera.look;

	if (trueHealth < apparentHealth)
		Fall(apparentHealth, dt * 50, trueHealth);
	else if (trueHealth > apparentHealth)
		Rise(apparentHealth, dt * 50, trueHealth);

	stepRate += dt;

	for (unsigned i = 0; i < 255; ++i)
	{
		myKeys[i] = false;
	}
}

float Player::get_apparentHP(float upon)
{
	return (apparentHealth/100)*upon;
}
float Player::get_trueHP(float upon)
{
	return (trueHealth/100)*upon;
}
void Player::reduceHealth(float reduction)
{
	trueHealth -= reduction;

	if(trueHealth < 1)
		trueHealth = 0;
}
void Player::recoverHealth(float recovery)
{
	trueHealth += recovery;

	if(trueHealth > 100)
		trueHealth = 100;
}