#include "pch.h"
#include "Camera.h"

namespace dae {
	void Camera::Update(const Timer* pTimer)
	{
		const float deltaTime = pTimer->GetElapsed();

		//Camera Update Logic

		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


		//Mouse Input
		int mouseX{}, mouseY{};
		const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

		// Transform Rotation:
		bool rotated = false;
		Vector3 movementDirection = {};

		if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) && abs(mouseY) > 1) {
			movementDirection += mouseY > 0 ? up : -up;
		}
		else {
			if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) || mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) && abs(mouseX) > 1) {
				float rotationDist = mouseX > 0 ? SDL_clamp(mouseX / 800.f, -3, -1) : SDL_clamp(mouseX / 800.f, 1, 3);
				totalYaw -= rotationDist * ROTATION_SPEED;
				totalYaw = fmod(totalYaw, 360.f);
				rotated = true;
			}

			if ((mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) && abs(mouseY) > 1) {
				float rotationDist = mouseY > 0 ? SDL_clamp(mouseY / 800.f, -3, -1) : SDL_clamp(mouseY / 800.f, 1, 3);
				totalPitch += rotationDist * ROTATION_SPEED;
				totalPitch = fmod(totalPitch, 360.f);
				rotated = true;
			}
		}

		if (rotated) {
			Matrix finalRotation = Matrix::CreateRotation(totalPitch, totalYaw, 0);
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
		}


		// Transform Position
		if (pKeyboardState[SDL_SCANCODE_W] == 1)
			movementDirection = forward;
		if (pKeyboardState[SDL_SCANCODE_A] == 1)
			movementDirection -= right;
		if (pKeyboardState[SDL_SCANCODE_S] == 1)
			movementDirection -= forward;
		if (pKeyboardState[SDL_SCANCODE_D] == 1)
			movementDirection += right;
		if (pKeyboardState[SDL_SCANCODE_Q] == 1)
			movementDirection -= up;
		if (pKeyboardState[SDL_SCANCODE_E] == 1)
			movementDirection += up;

		if (movementDirection.Magnitude() > 0.001) {
			movementDirection.Normalize();

			origin += movementDirection * (deltaTime * movementSpeed);
		}

		else {
			if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && abs(mouseY) > 0.001) {
				float movementDist = mouseY > 0 ? SDL_clamp(mouseY / 800.f, -3, -1) : SDL_clamp(mouseY / 800.f, 1, 3);
				origin += forward * (deltaTime * movementSpeed * movementDist);
			}
		}

		//Update Matrices
		CreateViewMatrix();
	}
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
