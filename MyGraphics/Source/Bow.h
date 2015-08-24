#include <iostream>
#include "Item.h"

using namespace std;

#pragma once
class CBow : public CWeapon
{
public:
	CBow(void);
	~CBow(void);

	Vector3 translation;
	float Charge;

	void Bob(double dt);
	void BobY(double dt);

	void Update(double dt);
	void RenderItem(MS& modelStack);
	void RenderArrow(MS& modelStack);

	float getCharge();
	void setCharge(float Charge);
};

