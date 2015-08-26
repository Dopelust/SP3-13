#include <iostream>
#include "Item.h"

using namespace std;

#pragma once
class CKnife : public CWeapon
{
public:
	CKnife(void);
	~CKnife(void);

	float currentWeapbobX;
	float currentWeapbobY;
	float weapbobAmountX;
	float weapbobAmountY;
	float stabForward;
	float Rotating;
	float takeoutRot;
	float takeoutRise;
	bool takeout;
	bool Stabbing;


	void Bob(double dt);
	void Stab(double dt);

	Vector3 translation;

	void Update(double dt);
	void RenderItem(MS& modelStack);
};

