#include "Knife.h"
#include "Application.h"

CKnife::CKnife(void) : translation(1, -0.5f, -1.5f)
{
	currentWeapbobX = 0;
	currentWeapbobY = 0;
	weapbobAmountX = 0;
	weapbobAmountY = 0;
	movement = 0;
	jabRot = 0;
	jump = false;
}

CKnife::~CKnife(void)
{
}

void CKnife::Update(double dt)
{

	if (Application::IsKeyPressed(VK_CONTROL))
	{
		movement += (float)(500 * dt);
	}
	else
	{
		movement += (float)(380 * dt);
	}

	weapbobAmountX = 3;
	weapbobAmountY = 10;
	currentWeapbobX = sin(Math::DegreeToRadian(movement)) * weapbobAmountX * 0.01;
	currentWeapbobY = cos(Math::DegreeToRadian(movement) * 2) * weapbobAmountY * 0.01;
	//find the distance
	Vector3 distance, jabDist;
	distance.x = 1.5f - translation.x;
	distance.y = -0.5f - translation.y;
	distance.Normalize();

	jabDist.z = -3.f - translation.z;
	jabDist.Normalize();

	//if (Application::IsMousePressed(1))
	//	Rise(Charge, dt, 1);
	//else
	//{
	//	if (Charge > 0)
	//		use = true;
	//	Charge = 0.f;
	//}

	if (Application::IsKeyPressed('W') || Application::IsKeyPressed('A') || Application::IsKeyPressed('S') || Application::IsKeyPressed('D'))
	{
		translation.x = 1 + currentWeapbobX;
		translation.y = -0.5f + currentWeapbobY;
	}

	if (!Application::IsKeyPressed('W') || !Application::IsKeyPressed('A') || !Application::IsKeyPressed('S') || !Application::IsKeyPressed('D'))
	{
		if (translation.x > 1.5)
		{
			translation.x += distance.x * dt;
		}
		if (translation.y > -0.5)
		{
			translation.y += distance.y * dt;
		}
	}


}

void CKnife::RenderItem(MS& modelStack)
{
	modelStack.Translate(translation.x, translation.y, translation.z);
	modelStack.Rotate(jabRot, -1, 0, 0);
	modelStack.Scale(0.2, 0.2, 0.2);
}
