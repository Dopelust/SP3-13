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
	float movement;
	int delay;
	float jabRot;
	bool startJab;
	bool endJab;
	bool jump;

	Vector3 translation;

	void Update(double dt);
	void RenderItem(MS& modelStack);
};

