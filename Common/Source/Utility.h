#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#include "Vector3.h"

void Fall(float& value, float by, float until);
void Rise(float& value, float by, float until);
void Knockback(Vector3& velocity, Vector3& velocityOffset, Vector3 direction);

bool IsEquals(int a, int b[], unsigned bSize);

class Spawner
{
public:
	Spawner();
	~Spawner();

	float timer;
	float rate;
	int count;
	int max;

	void Set(float rate, int max);
	bool Update(double dt);
};

class Oscillator
{
public:
	Oscillator();
	~Oscillator();

	float value;
	bool oscillate;

	float getValueSIN(float multiplier, float max);
	float getValueCOS(float multiplier, float max, float offset);
	float getValueNegativeCOS(float multiplier, float max, float offset);
	void Oscillate(float delta, int stop);
};

#endif