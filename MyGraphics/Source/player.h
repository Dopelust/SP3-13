/******************************************************************************/
/*!
\file	player.h
\author Ricsson
\par	
\brief
This is the player .h file
*/
/******************************************************************************/
#ifndef PLAYER_H
#define PLAYER_H

#include "StateMachine.h"
#include "Utility.h"
#include "Entity.h"
#include "Camera3.h"

#include <vector>
#include <string>
#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib")

using namespace irrklang;

class Player : public Entity
{
public:
	Player(){}
	~Player(){}

	float WALK_SPEED;
	Camera3 camera;

	float eyeLevel;
	
	void Init();
	void Update(double dt, const vector<Block*>&object, bool RestrictMovement);
	void UpdateVelocity(double dt);

	float apparentHealth;
	float trueHealth;
	float get_apparentHP(float upon);
	float get_trueHP(float upon);
	void recoverHealth(float recovery);
	void reduceHealth(float reduction);

	bool myKeys[255];
	bool noClip;
};

#endif