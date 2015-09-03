#include "Item.h"
#include "Application.h"

CItem::CItem(void) : mesh(NULL), count(0), unique(false), use(false), BobYAmount(45), bobY(0)
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

CFood::CFood(void) : HP(0), translation(0.7f, -0.5f, -1.f), Charge(0.f), useCD(0.3f)
{
	itemID = CItem::FOOD;
}

CFood::~CFood(void)
{
}

void CFood::setHP(unsigned HP)
{
	this->HP = HP;
}

unsigned int CFood::getHP(void)
{
	return HP;
}

void CFood::setCharge(float Charge)
{
	this->Charge = Charge;
}

void CFood::Update(double dt)
{
	if (count > 0 && Application::IsMousePressed(1) && useCD >= 0.3f)
	{
		Rise(Charge, dt, 1);
		BobY(dt);

		if (Charge == 1 && !use)
		{
			use = true;
			useCD = 0;
		}
	}
	else
		Charge = 0;

	useCD += dt;
}

void CFood::Bob(double dt)
{
	dt *= 350;
	currentWeapbobX = sin(Math::DegreeToRadian(dt)) * 0.02f;
	currentWeapbobY = cos(Math::DegreeToRadian(dt) * 2) * 0.05f;
}

void CFood::BobY(double dt)
{
	BobYAmount += dt * 600;
	bobY = cos(Math::DegreeToRadian(BobYAmount) * 2) * 0.03f;
}

void CFood::RenderItem(MS& modelStack)
{
	float C = Charge * 8;
	C = C > 1 ? 1 : C;

	modelStack.Translate(0, translation.y, translation.z);
	modelStack.Translate((currentWeapbobX + translation.x) * (1 - C), currentWeapbobY * (1 - C), 0);
	modelStack.Translate(0, bobY * C, 0);
	modelStack.Rotate(-45 * (1 - C), 0, 1, 0);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Scale(2, 4, 2);
}

