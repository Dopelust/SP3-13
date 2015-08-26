#include "Minimap.h"
#include "Application.h"

Waypoint::Waypoint() : selected(false), color(1,1,1)
{
}
Waypoint::~Waypoint()
{
}

Vector3 Waypoint::getWaypoint(float currentScaleX, float currentScaleY, float intendedScaleX, float intendedScaleY)
{
	return Vector3(Application::m_width * 0.5f + -point.x / currentScaleX * intendedScaleX, Application::m_height * 0.5f + point.y / currentScaleY * intendedScaleY, 0);
}

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