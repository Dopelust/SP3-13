#include "Bow.h"
#include "Application.h"

#include "Bow.h"
#include "Application.h"

CBow::CBow(void) : translation(1.6f, -0.5f, -2.6f)
{
	currentWeapbobX = 0;
	currentWeapbobY = 0;
	bowBobY = 0;
	BobYAmount = 45;

	itemID = CItem::BOW;
}

CBow::~CBow(void)
{
}

void CBow::Update(double dt)
{
	if (Application::IsMousePressed(1))
	{
		Rise(Charge, dt, 1);

		if (Charge > 0.5f)
			BobY(dt);
	}
	else
	{
		if (Charge > 0.5f && !use)
			use = true;
		else
			Charge = 0.f;
	}
}

void CBow::Bob(double dt)
{
	dt *= 250; 
	currentWeapbobX = sin(Math::DegreeToRadian(dt)) * 0.1f;
	currentWeapbobY = cos(Math::DegreeToRadian(dt) * 2) * 0.05f;
}

void CBow::BobY(double dt)
{
	BobYAmount +=  dt * 150;
	bowBobY = cos(Math::DegreeToRadian(BobYAmount) * 2) * 0.03f;
}


void CBow::RenderItem(MS& modelStack)
{
	float C = Charge;
	if (C < 0.5f)
		C = 0;
	else
		C -= 0.5f;

	modelStack.Translate(translation.x + currentWeapbobX, translation.y + currentWeapbobY + bowBobY, translation.z);
	modelStack.Rotate(15, 0, 1, 0);
	modelStack.Rotate(15, 1, 0, 0);
	modelStack.Scale(1, 0.75f, 1 + C);
}

void CBow::RenderArrow(MS& modelStack)
{
	float C = Charge * 2;
	C = C > 1 ? 1 : C;

	modelStack.Translate((C * translation.x) + currentWeapbobX, (C * translation.y) + currentWeapbobY + bowBobY, translation.z - 0.4f + Charge);
	modelStack.Rotate((C * 15), 0, 1, 0);
	modelStack.Rotate((C * 10), 1, 0, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Scale(3);
}

float CBow::getCharge()
{
	return Charge;
}

void CBow::setCharge(float Charge)
{
	this->Charge = Charge;
}
