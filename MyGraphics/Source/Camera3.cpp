/******************************************************************************/
/*!
\file	Camera3.cpp
\author Ricsson
\par	
\brief
This is the camera3 cpp
*/
/******************************************************************************/
#include "Camera3.h"
#include "MinScene.h"
#include "Application.h"
#include "Mtx44.h"

/******************************************************************************/
/*!
		Camera3:
\brief	This is the constructor
*/
/******************************************************************************/
Camera3::Camera3()
{
	orientation = 0.f;
	look = 0.f;

	fov = 70.f;
}

/******************************************************************************/
/*!
		Camera3:
\brief	This is the decstructor
*/
/******************************************************************************/
Camera3::~Camera3()
{
}

/******************************************************************************/
/*!
		Camera3:
\brief	This is the initializer
*/
/******************************************************************************/
void Camera3::Init(const Vector3& pos, const float& hO, const float& vO)
{
	orientation_offset = 0.f; look_offset = 0.f;

	orientation = defaultOrientation = hO + 0.1f;
	look = defaultLook = vO;

	this->position = defaultPosition = pos;
	Vector3 view; view.SphericalToCartesian(orientation, look);
	Vector3 right; right.SphericalToCartesian(orientation - 90.f, 0.0f);
	Vector3 direction; direction.SphericalToCartesian(orientation, 0.f);
	this->target = defaultTarget = position + direction;
	this->up = defaultUp = right.Cross(view);

	for (unsigned i = 0; i < 255; i++)
	{
		myKeys[i] = false;
	}
}

/******************************************************************************/
/*!
		Camera3:
\brief	This is the zoom function when sprinting and CCTV camera zoom
*/
/******************************************************************************/
void Camera3::Zoom(double dt)
{
	fov -=float( dt * 250 * Application::mouseScroll );

	if (fov < 10.f)
		fov = 10.f;

	if (fov > 70.f)
		fov = 70.f;
}

void Camera3::UpdateStatus(const unsigned char key)
{
	myKeys[key] = true;
}

/******************************************************************************/
/*!
		Camera3: Move
\brief	This is for the moving of the camera.
*/
/******************************************************************************/
void Camera3::Move(double dt)
{
	double x, y;
	Application::GetCursorPos(&x, &y);

	float yaw = (float)((float)(double(Application::m_width)/2 - x) * (fov/70.f));
	orientation += yaw/9.f;

	if (look <= 89 && look >= -89)
	{
		float pitch = (float)((float)(double(Application::m_height) / 2 - y) * (fov / 70.f));
		look += pitch/9.f;
	}

	if (look >= 89.f)
		look = 89.f;
	if (look <= -89.f)
		look = -89.f;

	if (look_offset > 0)
		Fall(look_offset, 10 * dt, 0);
	else if (look_offset < 0)
		Rise(look_offset, 10 * dt, 0);

	if (orientation_offset > 0)
		Fall(orientation_offset, 10 * dt, 0);
	else if (orientation_offset < 0)
		Rise(orientation_offset, 10 * dt, 0);
}

/******************************************************************************/
/*!
		Camera3:
\brief	This is the update function
*/
/******************************************************************************/

void Camera3::Update()
{
	Vector3 view; Vector3 right;
	view.SphericalToCartesian(orientation + orientation_offset, look + look_offset);
	right.SphericalToCartesian(orientation + orientation_offset - 90.f, 0.0f);
	up = right.Cross(view); 
	//up.Set(0, 1, 0);

	target = position + view;
	
	for (unsigned i = 0; i < 255; i++)
	{
		myKeys[i] = false;
	}
}

void Camera3::Track(Vector3 t, Vector3 rate)
{
	Vector3 dir = (t - position).Normalize();

	position += dir * rate;
	target += dir * rate;
}

void Camera3::Reset()
{
	orientation = defaultOrientation;
	look = defaultLook;

	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;

	fov = 70;
}