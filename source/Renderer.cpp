#include "pch.h"
#include "Renderer.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
			return;
		}

		// Initialise the mesh.
		m_Mesh = new TriangleMesh(m_pDevice);
	}

	Renderer::~Renderer()
	{
		if (m_pRenderTargetView) {
			m_pRenderTargetView->Release();
		}
		if (m_pRenderTargetBuffer) {
			m_pRenderTargetBuffer->Release();
		}
		if (m_pDepthStencilView) {
			m_pDepthStencilView->Release();
		}
		if (m_pDepthStencilBuffer) {
			m_pDepthStencilBuffer->Release();
		}
		if (m_pSwapChain) {
			m_pSwapChain->Release();
		}
		if (m_pDeviceContext) {
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pDevice) {
			m_pDevice->Release();
		}

		delete m_Mesh;
	}

	void Renderer::Update(const Timer* pTimer)
	{

	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		// Clear buffers (RTV and DSV).
		constexpr float color[4] = {0.f, 0.f, 0.3f, 1.f};
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);	// Clear the Render Target
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);	// Clear both depth and stencil buffer and set to 1 and 0 respectively.

		// Actually render. Set pipeline and invoke draw calls.
		m_Mesh->Render(m_pDeviceContext);

		// Present the back buffer aka swap.
		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		// Create Device and Device Context
		// ================================

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;	// Set it to DirectX 11.1.
		uint32_t createDeviceFlags = 0;								// Set up flags for devices used.

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;				// Flag debug layer of the soon-to-be-created device as used.
#endif

		// Create the devices necessary.
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		// Make sure the creation did not fail.
		if (FAILED(result))
			return result;

		// Create Swap Chain
		// ================================

		// Create a Factory for the swap chain to save memory and improve performance.
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		// Make sure the creation did not fail.
		if (FAILED(result))
			return result;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		// Need to create a description so the device knows what kind of swap chain to create down the line. Are the settings.
		swapChainDesc.BufferDesc.Width = m_Width;						// Width of the buffers.
		swapChainDesc.BufferDesc.Height = m_Height;						// Height of the buffers.
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;				// The numerator of the refresh rate. Gets combined with the denominator for the Hz refresh rate.
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;			// The denominator of the refresh rate. Gets combined with the numerator for the Hz refresh rate.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// Typeless 32-bit RGBA format for colour.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// How the scanlines are created. In this case unspecified.
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;					// What happens if the image is stretched on the monitor. Unspecified prevents accidental switching between modes.
		swapChainDesc.SampleDesc.Count = 1;								// The number of multisamples per pixel.
		swapChainDesc.SampleDesc.Quality = 0;							// The image quality of multisampling. Between 0 and max as defined by the device.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// What is the usage of the bugger. In our case, used as output render target.
		swapChainDesc.BufferCount = 1;									// Number of buffers in the chain.
		swapChainDesc.Windowed = true;									// Output is in windowed mode.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			// What happens to the contents of the buffer after presenting a surface.
		swapChainDesc.Flags = 0;										// Other less common options. 0 is none apply.

		// Get the handle to the window from the OS from SDL.
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		// Create the actual swap chain.
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		// Make sure the creation did not fail.
		if (FAILED(result))
			return result;

		// Avoid the resource leak from the factory.
		pDxgiFactory->Release();

		// Create Depth Stencil and Depth Stencil View.
		// ================================
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;								// Width of the buffers.
		depthStencilDesc.Height = m_Height;								// Height of the buffers.
		depthStencilDesc.MipLevels = 1;									// Maximum number of mipmap levels in the texture.
		depthStencilDesc.ArraySize = 1;									// Number of textures in texture array. e.g. in case of cube maps. We're just using a simple 2D texture.
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// Z-buffer format supporting 24 bits for depth and 8 for stencil.
		depthStencilDesc.SampleDesc.Count = 1;							// The number of multisamples per pixel.
		depthStencilDesc.SampleDesc.Quality = 0;						// The image quality of multisampling. Between 0 and max as defined by the device.
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;					// How the texture is to be read from and written to. Default: Read and Write access by GPU only.
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;			// Flags for binding resources to the pipeline for the output-merger stage. Flags this as a depth-stencil target.
		depthStencilDesc.CPUAccessFlags = 0;							// Types of CPU access allowed. 0 is not required.
		depthStencilDesc.MiscFlags = 0;									// Other less common resource options. 0 if none apply.

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;			// Z-buffer format again.
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;		// How the resource will be accessed. Will be accessed as a normal 2D Texture without multisampling.
		depthStencilViewDesc.Texture2D.MipSlice = 0;					// Index of the first mipmap level to use.

		// Create both resources.
		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;
		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		// Create Render Target and Render Target View
		// ================================
		
		// Render target itself.
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		// View of the render target.
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		// Bind the Depth Buffer and Render Target during Output Merger Stage.
		// ================================
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// Set Viewport
		// ================================
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}
}
