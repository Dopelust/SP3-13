#include "Utility.h"

using namespace::std;

bool IsEquals(int a, int b[], unsigned bSize)
{
	for (unsigned i = 0; i < bSize; ++i)
	{
		if (a == b[i])
			return true;
	}
	return false;
}

void Fall(float& value, float by, float until)
{
	value = value - by < until ? until : value - by;
}

void Rise(float& value, float by, float until)
{
	value = value + by > until ? until : value + by;
}

void Knockback(Vector3& velocity, Vector3& velocityOffset, Vector3 direction)
{
	velocityOffset.x += direction.x;
	velocity.y += direction.y;
	velocityOffset.z += direction.z;
}

Oscillator::Oscillator()
{
	value = 0;
	oscillate = true;
}

Oscillator::~Oscillator()
{
}

float Oscillator::getValueSIN(float multiplier, float max)
{
	return sin(Math::DegreeToRadian(value) * multiplier) * max;
}

float Oscillator::getValueCOS(float multiplier, float max, float offset)
{
	return offset + cos(Math::DegreeToRadian(value) * multiplier) * max;
}
float Oscillator::getValueNegativeCOS(float multiplier, float max, float offset)
{
	return offset + -cos(Math::DegreeToRadian(value) * multiplier) * max;
}

void Oscillator::Oscillate(float delta, int stop)
{
	if (oscillate)
	{
		value += delta;
	}

	else
	{
		if ((int)value % stop != 0)
		{
			int stopValuePOS = (int)value;
			int stopValueNEG = (int)value;
			int countPOS = 0;
			int countNEG = 0;

			while (stopValuePOS % stop != 0)
			{
				stopValuePOS++; countPOS++;
			}
			while (stopValueNEG % stop != 0)
			{
				stopValueNEG--; countNEG++;
			}

			if (countPOS < countNEG)
				Rise(value, delta/2.f, stopValuePOS);
			else
				Fall(value, delta/2.f, stopValueNEG);
		}
	}
}

Spawner::Spawner()
{
	rate = 0.f;
	timer = 0.f;
	count = 0;
	max = 0;
}
void Spawner::Set(float rate, int max)
{
	this->rate = rate;
	this->max = max;
}
Spawner::~Spawner()
{
}
bool Spawner::Update(double dt)
{
	timer += dt;

	if (timer >= rate)
	{
		if (count < max)
		{
			timer = 0.f;
			return true;
		}
	}

	return false;
}