#pragma once

namespace dae {
	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

		Camera(const Camera&) = delete;
		Camera(Camera&&) noexcept = delete;
		Camera& operator=(const Camera&) = delete;
		Camera& operator=(Camera&&) noexcept = delete;

		Matrix GetViewMatrix() { return viewMatrix; };
		Matrix GetProjectionMatrix() { return projectionMatrix; };
		
		void Update(const Timer* pTimer);
		void Initialize(float _fovAngle, Vector3 _origin, int _width, int _height, float _zn, float _zf);

		const float ROTATION_SPEED = .05f;
		float movementSpeed = 10.f;
		float movementSpeedMouse = 50.f;
		Vector3 origin{};

	private:
		float fovAngle{ 90.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };
		float zn{};
		float zf{};

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		void CreateProjectionMatrix(int width, int height);
		void CreateViewMatrix();
		Matrix CalculateCameraToWorld();
	};
}

