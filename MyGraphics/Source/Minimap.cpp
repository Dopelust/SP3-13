#include "Minimap.h"
#include "vertex.h"
#include "MyMath.h"
#include <GL\glew.h>
#include <vector>

Minimap::Minimap(void)
	: Background(NULL)
	, Border(NULL)
	, Avatar(NULL)
{
	radius = 1;
	maxDist = 100;
	scale = 1;
}
Minimap::~Minimap(void)
{
	delete Background;
	delete Border;
	delete Avatar;
}

void Minimap::SetRadius(const float radius)
{
	this->radius = radius;
}
float Minimap::GetRadius()
{
	return radius;
}
float Minimap::GetDiameter()
{
	return radius*2;
}
void Minimap::SetMaxDistFromPlayer(const float maxDist)
{
	this->maxDist = maxDist;
}
float Minimap::GetMaxDistFromPlayer()
{
	return maxDist;
}
void Minimap::SetScale(const float scale)
{
	this->scale = scale;
}
float Minimap::GetScale()
{
	return scale;
}