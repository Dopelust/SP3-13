#pragma once
#include <iostream>
#include <string>
#include "Mesh.h"
#include "MatrixStack.h"

using namespace::std;

class CItem
{
public:
	enum ItemIDs
	{
		KNIFE = 1,
		BOW,
		FOOD,
		NUM_ITEMS,
	};

	CItem(void);
	~CItem(void);

	string name;
	Mesh* mesh;
	unsigned count;
	bool unique;
	bool use;
	unsigned itemID;

	virtual void Update(double dt);
	virtual void RenderItem(MS& modelStack);
	virtual void Bob(double dt);

	virtual float getCharge();
	virtual void setCharge(float Charge);
};

class CWeapon : public CItem
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

	virtual void Update(double dt);
	virtual void RenderItem(MS& modelStack);
	virtual void Bob(double dt);

	virtual float getCharge();
	virtual void setCharge(float Charge);
};

