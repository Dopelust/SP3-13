#include <iostream>
#include "Weapon.h"

using namespace std;

#pragma once
class CKnife : public CWeapon
{
public:
	CKnife(void);
	~CKnife(void);

	void Attack();
};

