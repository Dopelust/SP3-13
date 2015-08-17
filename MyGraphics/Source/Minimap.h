#ifndef MINIMAP_H
#define MINIMAP_H

#include "Mesh.h"

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