#include "Bow.h"
#include "Application.h"

#include "Bow.h"
#include "Application.h"

CBow::CBow(void) : translation(1.6f, -0.5f, -2.6f)
{
	currentWeapbobX = 0;
	currentWeapbobY = 0;

	itemID = CItem::BOW;
}

CBow::~CBow(void)
{
}

void CBow::Update(double dt)
{
	if (Application::IsMousePressed(1))
		Rise(Charge, dt, 1);
	else
	{
		if (Charge > 0.3f && !use)
			use = true;
		else
			Charge = 0.f;
	}
}

void CBow::Bob(double dt)
{
	dt *= 250; 
	currentWeapbobX = sin(Math::DegreeToRadian(dt)) * 0.15f;
	currentWeapbobY = cos(Math::DegreeToRadian(dt) * 2) * 0.05f;
}

void CBow::RenderItem(MS& modelStack)
{
	modelStack.Translate(translation.x + currentWeapbobX, translation.y + currentWeapbobY, translation.z + Charge * 1.f);
	modelStack.Rotate(15, 0, 1, 0);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Scale(1, 0.75f, 1);
}

float CBow::getCharge()
{
	return Charge;
}

void CBow::setCharge(float Charge)
{
	this->Charge = Charge;
}
