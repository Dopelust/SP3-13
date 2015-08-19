#include <iostream>
#include "Weapon.h"

using namespace std;

#pragma once
class CBow : public CWeapon
{
public:
	CBow(void);
	~CBow(void);

	Vector3 translation;
	float Charge;

	void Update(double dt);
	void RenderItem(MS& modelStack);
};

