#ifndef MINIMAP_H
#define MINIMAP_H

#include "Vector3.h"
#include "Mesh.h"
#include <string>

using namespace::std;

class Waypoint
{
public:
	Waypoint();
	~Waypoint();

	Vector3 getWaypoint(float currentScaleX, float currentScaleY, float intendedScaleX, float intendedScaleY);

	Vector3 point;
	Color color;
	unsigned texture;
	string name;
	bool selected;
};

class Minimap
{
public:
	Minimap(void);
	virtual ~Minimap(void);

	Mesh* Background;
	Mesh* Border;
	Mesh* Avatar;

	void SetRadius(const float radius);
	float GetRadius();
	float GetDiameter(); 

	void SetMaxDistFromPlayer(const float maxDist);
	float GetMaxDistFromPlayer();

	void SetScale(const float scale);
	float GetScale();

private:
	float radius;
	float maxDist;
	float scale;
};

#endif