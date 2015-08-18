/******************************************************************************/
/*!
\file	Weapon.h
\author Lazarus
\par	
\brief
This is a weapon class to create weapons.(Name,Damage,Type)
*/
/******************************************************************************/
#include <iostream>
#include <string.h>
#pragma once
#include "Item.h"
using namespace std;

class CWeapon : CItem
{
private:
	unsigned DMG;
	unsigned ammo;
public:
	CWeapon(void);
	~CWeapon(void);

	//set properties
	void setDamage(unsigned DMG);
	void setAmmo(unsigned ammo);
	unsigned getDamage();
	unsigned getAmmo();

	virtual void Attack();
};
