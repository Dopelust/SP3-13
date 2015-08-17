#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include "Vector3.h"
#include "Block.h"

class Plane
{
public:
	Vector3 normal, point;
	float d;

	Plane::Plane(Vector3 &v1, Vector3 &v2, Vector3 &v3);
	Plane::Plane(void);
	Plane::~Plane();

	void set3Points(Vector3 &v1, Vector3 &v2, Vector3 &v3);
	void setNormalAndPoint(Vector3 &normal, Vector3 &point);
	void setCoefficients(float a, float b, float c, float d);
	float distance(Vector3 &p);
};

class FrustumG {

private:

	enum {
		TOP = 0, BOTTOM, LEFT,
		RIGHT, NEARP, FARP
	};

public:

	static enum { OUTSIDE, INTERSECT, INSIDE };
	Plane pl[6];
	Vector3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;

	FrustumG::FrustumG();
	FrustumG::~FrustumG();

	void setCamInternals(float angle, float ratio, float nearD, float farD);
	void setCamDef(Vector3 &p, Vector3 &l, Vector3 &u);
	int pointInFrustum(Vector3 &p);
	int sphereInFrustum(Vector3 &p, float raio);
	int boxInFrustum(Block &b);
};

#endif