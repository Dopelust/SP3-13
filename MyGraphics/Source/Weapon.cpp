#include "Weapon.h"


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

void CWeapon::Attack()
{
}

