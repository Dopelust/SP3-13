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
#include "Living.h"
#include "Camera3.h"
#include "Inventory.h"

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

	Camera3 camera;
	CInventory inventory;
	float WALK_SPEED;

	float eyeLevel;
	
	void Init();
	void Update(double dt, bool RestrictMovement);
	void UpdateVelocity(double dt);

	float maxHealth;
	void recoverHealth(float recovery);
	void reduceHealth(float reduction);

	bool myKeys[255];
	bool noClip;

	float SprintBar;
	float MaxSprintTime;
	//Laz
	int SprintCounter;
	bool Sprint;
	bool run;
	float Horse;
	float upDown;
	bool Riding;

	//Skill Tree
	int skillPoint;
	float damageCounter;
	float speedCounter;
	bool addHealth;
	bool addSpeed;
	bool addDamage;
	bool addStamina;
	void UpdateSkilltree(double dt);

	//Exp LAZ
	int Lv;
	float maxEXP;
	float currentEXP;
	void updateEXP(double dt);
	

	CItem* getSelectedItem();
};

#endif