#include "Frustum.h"

/* ------------------------------------------------------

View Frustum - Lighthouse3D

-----------------------------------------------------*/

#include "Frustum.h"
#include <math.h>

#define ANG2RAD 3.14159265358979323846/180.0

FrustumG::FrustumG() {}

FrustumG::~FrustumG() {}

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD) {

	this->ratio = ratio;
	this->angle = angle;
	this->nearD = nearD;
	this->farD = farD;

	tang = (float)tan(angle* ANG2RAD * 0.5);
	nh = nearD * tang;
	nw = nh * ratio;
	fh = farD  * tang;
	fw = fh * ratio;
}


void FrustumG::setCamDef(Vector3 &p, Vector3 &l, Vector3 &u) {

	Vector3 dir, nc, fc, X, Y, Z;

	Z = p - l;
	Z.Normalize();
	X = u * Z; 
	X.Normalize();
	Y = Z * X;

	nc = p - Z * nearD;
	fc = p - Z * farD;

	ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;

	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;

	pl[TOP].set3Points(ntr, ntl, ftl);
	pl[BOTTOM].set3Points(nbl, nbr, fbr);
	pl[LEFT].set3Points(ntl, nbl, fbl);
	pl[RIGHT].set3Points(nbr, ntr, fbr);
	pl[NEARP].set3Points(ntl, ntr, nbr);
	pl[FARP].set3Points(ftr, ftl, fbl);
}


int FrustumG::pointInFrustum(Vector3 &p) {

	int result = INSIDE;
	for (int i = 0; i < 6; i++) {

		if (pl[i].distance(p) < 0)
			return OUTSIDE;
	}
	return(result);

}


int FrustumG::sphereInFrustum(Vector3 &p, float raio) {

	int result = INSIDE;
	float distance;

	for (int i = 0; i < 6; i++) {
		distance = pl[i].distance(p);
		if (distance < -raio)
			return OUTSIDE;
		else if (distance < raio)
			result = INTERSECT;
	}
	return(result);

}


int FrustumG::boxInFrustum(Block &b) {

	//int result = INSIDE;
	//for (int i = 0; i < 6; i++) 
	//{
	////	Vector3 minCoord = b.getMinCoord();
	////	Vector3 maxCoord = b.getMaxCoord();

	//	if (pl[i].normal.x >= 0)
	//	{
	//		float temp = minCoord.x;
	//		minCoord.x = maxCoord.x; maxCoord.x = temp;
	//	}
	//	if (pl[i].normal.y >= 0)
	//	{
	//		float temp = minCoord.y;
	//		minCoord.y = maxCoord.y; maxCoord.y = temp;
	//	}
	//	if (pl[i].normal.z >= 0)
	//	{
	//		float temp = minCoord.z;
	//		minCoord.z = maxCoord.z; maxCoord.z = temp;
	//	}

	//	if (pl[i].distance(maxCoord) < 0)
	//		return OUTSIDE;
	//	else if (pl[i].distance(minCoord) < 0)
	//		result = INTERSECT;
	//}
	//return(result);

}

Plane::Plane(Vector3 &v1, Vector3 &v2, Vector3 &v3) {

	set3Points(v1, v2, v3);
}


Plane::Plane() {}

Plane::~Plane() {}


void Plane::set3Points(Vector3 &v1, Vector3 &v2, Vector3 &v3) {


	Vector3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	normal = aux2 * aux1;
	normal.Normalize();

	point = v2;
	d = -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
}

void Plane::setNormalAndPoint(Vector3 &normal, Vector3 &point) {

	this->normal = normal;
	this->normal.Normalize();
	d = -(normal.x * point.x + normal.y * point.y + normal.z * point.z);
}

void Plane::setCoefficients(float a, float b, float c, float d) {

	// set the normal vector
	normal.Set(a, b, c);
	//compute the lenght of the vector
	float l = normal.Length();
	// normalize the vector
	normal.Set(a / l, b / l, c / l);
	// and divide d by th length as well
	this->d = d / l;
}

float Plane::distance(Vector3 &p) {

	return (d + (normal.x * point.x + normal.y * point.y + normal.z * point.z));
}