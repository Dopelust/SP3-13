#include "Item.h"
#include "stdlib.h"


CItem::CItem(void) : mesh(NULL), count(0), unique(false), use(false)
{
}


CItem::~CItem(void)
{
}

void CItem::Update(double dt)
{
}

void CItem::RenderItem(MS & modelStack)
{
}

void CItem::Bob(double dt)
{
}

float CItem::getCharge()
{
	return 0;
}

void CItem::setCharge(float Charge)
{
}

CWeapon::CWeapon(void)
{
	unique = true;
}


CWeapon::~CWeapon(void)
{
}

void CWeapon::setDamage(unsigned DMG)
{
	this->DMG = DMG;
}
void CWeapon::setAmmo(unsigned ammo)
{
	this->ammo = ammo;
}
unsigned CWeapon::getDamage()
{
	return DMG;
}
unsigned CWeapon::getAmmo()
{
	return ammo;
}

void CWeapon::Update(double dt)
{
}

void CWeapon::RenderItem(MS& modelStack)
{
}

float CWeapon::getCharge()
{
	return 0;
}

void CWeapon::setCharge(float Charge)
{
}

void CWeapon::Bob(double dt)
{
}
