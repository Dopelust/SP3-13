#ifndef COLLISION_H
#define COLLISION_H

#include "Vector3.h"

class Collision2D
{
public:
	Collision2D();
	~Collision2D();

	Vector3 position;
	Vector3 hitbox;
	Vector3 centre;
	bool platform;

	Vector3 getMaxCoord();
	Vector3 getMinCoord();

	void Set(Vector3 position, Vector3 hitbox, Vector3 centre);

	static bool checkCollision(Collision2D a, Collision2D b); //AABB - AABB
	//bool collisionWithSphere(Vector3 point, float radius); //Sphere - AABB
	//float collisionWithRay(Vector3 point, Vector3 dir); //Ray - AABB

	static float xPenetrationDepth(Collision2D a, Collision2D b);
	static float zPenetrationDepth(Collision2D a, Collision2D b);
	static float yPenetrationDepth(Collision2D a, Collision2D b);
	static float PenetrationDepth(Collision2D a, Collision2D b); //Determines collision order for dynamic objects
};

#endif