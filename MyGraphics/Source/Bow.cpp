#include "Bow.h"
#include "Application.h"

CBow::CBow(void) : translation(1.5f, -0.3f, -2.5f)
{
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
		if (Charge > 0.2f && !use)
			use = true;
		else
		{
			Charge = 0.f;
		}
	}

	translation.z = -2.5f + (Charge * 1.1f);
}

void CBow::RenderItem(MS& modelStack)
{
	modelStack.Translate(translation);
	modelStack.Rotate(15, 1, 0, 0);
	//modelStack.Scale(0.1f);
}

float CBow::getCharge()
{
	return Charge;
}

void CBow::setCharge(float Charge)
{
	this->Charge = Charge;
}
