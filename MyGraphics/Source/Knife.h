#include <iostream>
#include "Item.h"

using namespace std;

#pragma once
class CKnife : public CWeapon
{
public:
	CKnife(void);
	~CKnife(void);

	void Update(double dt);
	void RenderItem(MS& modelStack);
};

