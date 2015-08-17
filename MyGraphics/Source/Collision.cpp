#include "Collision.h"

using namespace::std;

Collision2D::Collision2D() : position(0,0,0), hitbox(0,0,0), centre(0,0,0), platform(false)
{
}
Collision2D::~Collision2D()
{
}

void Collision2D::Set(Vector3 position, Vector3 hitbox, Vector3 centre)
{
	this->position = position;
	this->hitbox = hitbox;
	this->centre = centre;
}
Vector3 Collision2D::getMaxCoord()
{
	return position + centre + hitbox/2;
}
Vector3 Collision2D::getMinCoord()
{
	return position + centre - hitbox/2;
}

bool Collision2D::checkCollision(Collision2D a, Collision2D b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	return(maxCubeA.x > minCubeB.x && 
		minCubeA.x < maxCubeB.x && 
		maxCubeA.y > minCubeB.y && 
		minCubeA.y < maxCubeB.y && 
		maxCubeA.z > minCubeB.z && 
		minCubeA.z < maxCubeB.z);
}
/*
bool Collision::collisionWithSphere(Vector3 point, float radius)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	float dist_squared = radius * radius;

    if (point.x < minCube.x) 
		dist_squared -= (point.x - minCube.x) * (point.x - minCube.x);
    else if (point.x > maxCube.x) 
		dist_squared -= (point.x - maxCube.x) * (point.x - maxCube.x);
    if (point.y < minCube.y) 
		dist_squared -= (point.y - minCube.y) * (point.y - minCube.y);
    else if (point.y > maxCube.y) 
		dist_squared -= (point.y - maxCube.y) * (point.y - maxCube.y);
    if (point.z < minCube.z)
		dist_squared -= (point.z - minCube.z) * (point.z - minCube.z);
    else if (point.z > maxCube.z) 
		dist_squared -= (point.z - maxCube.z) * (point.z - maxCube.z);

    return dist_squared > 0;
}
float Collision::collisionWithRay(Vector3 point, Vector3 dir)
{
	Vector3 maxCube = getMaxCoord();
	Vector3 minCube = getMinCoord();

	// r.dir is unit direction vector of ray
	Vector3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;

	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (minCube.x - point.x)*dirfrac.x;
	float t2 = (maxCube.x - point.x)*dirfrac.x;
	float t3 = (minCube.y - point.y)*dirfrac.y;
	float t4 = (maxCube.y - point.y)*dirfrac.y;
	float t5 = (minCube.z - point.z)*dirfrac.z;
	float t6 = (maxCube.z - point.z)*dirfrac.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0)
		return -1;
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
		return -1;

	return tmin;
}
*/
float Collision2D::xPenetrationDepth(Collision2D a, Collision2D b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	if (maxCubeA.x > maxCubeB.x && minCubeA.x > minCubeB.x)
		return maxCubeB.x - minCubeA.x;

	else if (maxCubeA.x < maxCubeB.x && minCubeA.x < minCubeB.x)
		return maxCubeA.x - minCubeB.x;

	else if (minCubeA.x >= minCubeB.x && maxCubeA.x <= maxCubeB.x)
		return maxCubeA.x - minCubeA.x;

	else if (minCubeA.x <= minCubeB.x && maxCubeA.x >= maxCubeB.x)
		return maxCubeA.x - minCubeA.x;

	else 
		return 0;
}
float Collision2D::zPenetrationDepth(Collision2D a, Collision2D  b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	if (maxCubeA.z > maxCubeB.z && minCubeA.z > minCubeB.z)
		return maxCubeB.z - minCubeA.z;

	else if (maxCubeA.z < maxCubeB.z && minCubeA.z < minCubeB.z)
		return maxCubeA.z - minCubeB.z;

	else if (minCubeA.z >= minCubeB.z && maxCubeA.z <= maxCubeB.z)
		return maxCubeA.z - minCubeA.z;

	else if (minCubeA.z <= minCubeB.z && maxCubeA.z >= maxCubeB.z)
		return maxCubeA.z - minCubeA.z;
	else 
		return 0;
}
float Collision2D::yPenetrationDepth(Collision2D a, Collision2D b)
{
	Vector3 maxCubeA = a.getMaxCoord();
	Vector3 minCubeA = a.getMinCoord();
	Vector3 maxCubeB = b.getMaxCoord();
	Vector3 minCubeB = b.getMinCoord();

	if (maxCubeA.y > maxCubeB.y && minCubeA.y > minCubeB.y)
		return maxCubeB.y - minCubeA.y;

	else if (maxCubeA.y < maxCubeB.y && minCubeA.y < minCubeB.y)
		return maxCubeA.y - minCubeB.y;

	else if (minCubeA.y >= minCubeB.y && maxCubeA.y <= maxCubeB.y)
		return maxCubeA.y - minCubeA.y;

	else if (minCubeA.y <= minCubeB.y && maxCubeA.y >= maxCubeB.y)
		return maxCubeA.y - minCubeA.y;

	else 
		return 0;
}
float Collision2D::PenetrationDepth(Collision2D a, Collision2D  b)
{
	return xPenetrationDepth(a, b) * yPenetrationDepth(a, b);
}
