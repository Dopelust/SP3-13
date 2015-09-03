#include "Knife.h"
#include "Application.h"

extern ISoundEngine * engine;

CKnife::CKnife(void) : translation(1, -0.5f, -1.5f), attackTimer(0), attackRate(0.5f)
{
	currentWeapbobX = 0;
	currentWeapbobY = 0;
	stabForward = (-0.5);
	Stabbing = false;
	Rotating = 0;
	bool takeout = false;
	takeoutRot = 180;

	itemID = CItem::KNIFE;
}

CKnife::~CKnife(void)
{
}

void CKnife::Update(double dt)
{
	if (Application::IsMousePressed(0) && stabForward == 0 && attackTimer >= attackRate)
	{
		char* soundFileLocation[3] = { "Assets/Media/Weapons/Stab1.mp3", "Assets/Media/Weapons/Stab2.mp3" , "Assets/Media/Weapons/Stab3.mp3" };
		ISound* sound = engine->play2D(soundFileLocation[rand() % 3], false, true);
		if (sound)
		{
			sound->setVolume(0.2f);
			sound->setIsPaused(false);
		}
		Stabbing = true;
		use = true;
		attackTimer = 0;
	}
	if (Application::IsMousePressed(0) && stabForward == -3)
	{
		Stabbing = false;
	}
	if (!Application::IsMousePressed(0) && stabForward == -3)
	{
		Stabbing = false;
	}

	if (Stabbing == true)
	{
		Fall(stabForward, dt * 30, -3);
		Fall(Rotating, dt * 30, 90);
		translation.y = -1;
	}
	if (stabForward == 0)
	{
		Rise(Rotating, dt * 20, 0);
		translation.y = -0.5;
	}
	else if (Stabbing == false)
	{
		Rise(stabForward, dt * 20, 0);
		translation.y = -1;
	}

	if (Application::IsKeyPressed('1'))
	{
		takeout = true;
		//translation.y = -0;
	}
	if (takeout == true && Application::IsKeyPressed('1') || !Application::IsKeyPressed('1'))
	{
		Rise(takeoutRise, dt * 5, 1.5f);
		Fall(takeoutRot, dt * 300, 0);
		translation.y = -2;
		/*cout << takeoutRise << endl;
		cout << takeoutRot << endl;*/
	}
	if (!Application::IsKeyPressed('1') && takeoutRot == 0)
	{
		takeout = false;
	}

	if (takeout == false)
	{
		translation.y = -0.5f;
		takeoutRise = 0;
		takeoutRot = 180;
	}

	attackTimer += dt;
}

void CKnife::Bob(double dt)
{
	dt *= 350;
	currentWeapbobX = sin(Math::DegreeToRadian(dt)) * 0.015f;
	currentWeapbobY = cos(Math::DegreeToRadian(dt) * 2) * 0.05f;
}

/*void CKnife::Stab(double dt)
{

if(Application::IsMousePressed(0))
{
stabPeak = true;
stabMin = false;
cout << stabPeak << endl;
cout << stabMin << endl;
}
if(stabPeak = true)
{
stabForward ++;
}

}*/

void CKnife::RenderItem(MS& modelStack)
{
	modelStack.Translate(translation.x + currentWeapbobX, translation.y + currentWeapbobY + takeoutRise, translation.z + stabForward);
	modelStack.Rotate(Rotating, -1, 0, 0);
	if (takeout == true)
	{
		modelStack.Rotate(takeoutRot, -1, 0, 0);
	}
	modelStack.Scale(0.2);


}
