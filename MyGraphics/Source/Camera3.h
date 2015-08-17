/******************************************************************************/
/*!
\file	Camera3.h
\author Ricsson
\par	
\brief
Camera function that uses mouse control.
*/
/******************************************************************************/
#ifndef CAMERA_3_H
#define CAMERA_3_H

#include "Camera.h"
#include "Block.h"

#include <vector>

using namespace::std;

/******************************************************************************/
/*!
		Class Camera3: inherits from camera class.
\brief	Camera3 class
*/
/******************************************************************************/

class Camera3 : public Camera
{
public:
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	float defaultOrientation;
	float defaultLook;

	float fov;
	float orientation;
	float orientation_offset;
	float look;
	float look_offset;

	Camera3();
	~Camera3();
	virtual void Init(const Vector3& pos, const float& orientation, const float& look);
	virtual void Update();
	virtual void UpdateStatus(const unsigned char key);
	void Move(double dt);
	void Zoom(double dt);
	void Track(Vector3 t, Vector3 rate);
	virtual void Reset();

	bool myKeys[255];
};

#endif