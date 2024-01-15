#pragma once
#include "TriangleMesh.h"
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void changeCameraMovement(float delta);

		void Update(const Timer* pTimer);
		void Render() const;

		// Functions for cycling or toggling features.
		void CycleFilterMethod();
		void ToggleRotation();
		void ToggleNormalMap();
		void toggleFireMesh();

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};
		bool m_rotatingEnabled{ true };
		bool m_fireMeshEnabled{ true };

		// Initialized stuff.
		bool m_IsInitialized{ false };
		TriangleMesh* m_pMesh{ nullptr };
		TriangleMesh* m_pFireMesh{ nullptr };
		Camera* m_pCamera{ nullptr };

		//DIRECTX
		HRESULT InitializeDirectX();
		ID3D11Device* m_pDevice{ nullptr };						// Resource for creating and allocating resources. 
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };		// The context in which a device is used. Sets the pipeline state and uses the resources of the Device to generate rendering commands.

		IDXGISwapChain* m_pSwapChain{ nullptr };				// The swap chain holding the buffers to switch between what is presented and what is being calculated. (Front vs. back buffer.)
		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };		// The depth and stencil buffer.
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr }; // The view determining how the depth buffer is used in the pipeline.

		ID3D11Texture2D* m_pRenderTargetBuffer{ nullptr };		// The back buffer, in this case, aka render target, as we only have one.
		ID3D11RenderTargetView* m_pRenderTargetView{ nullptr }; // The view of the render target. Determines how the render target is used in the pipeline.
	};
}
