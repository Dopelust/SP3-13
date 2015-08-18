#include <iostream>
#include "Weapon.h"

using namespace std;

#pragma once
class CBow : public CWeapon
{
public:
	CBow(void);
	~CBow(void);

	void Attack();
};

