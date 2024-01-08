#include "pch.h"
#include "Camera.h"

namespace dae {
	void Camera::Initialize(float _fovAngle, Vector3 _origin, int _width, int _height, float _zn, float _zf)
	{
		fovAngle = _fovAngle;
		fov = tanf((fovAngle * TO_RADIANS) / 2.f);
		origin = _origin;

		zn = _zn;
		zf = _zf;

		CreateViewMatrix();
		CreateProjectionMatrix(_width, _height);
	}
	void Camera::CreateProjectionMatrix(int width, int height)
	{
		float aspect = (float)width / height;

		projectionMatrix = { {1.f / (aspect * fov),		0,			0,						0},
							{ 0,						1.f / fov,	0,						0 },
							{ 0,						0,			zf / (zf - zn),			1 },
							{ 0,						0,			-(zf * zn) / (zf - zn), 0 }
						};
	}
	void Camera::CreateViewMatrix()
	{
		Matrix invViewMatrix = CalculateCameraToWorld();
		viewMatrix = Matrix::Inverse(invViewMatrix);
	}

	Matrix Camera::CalculateCameraToWorld()
	{
		right = Vector3::Cross({ 0, 1, 0 }, forward.Normalized());
		right.Normalize();
		up = Vector3::Cross(forward, right);
		up.Normalize();

		return { right, up, forward.Normalized(), origin };
	}
}
