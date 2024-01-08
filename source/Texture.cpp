#include "assert.h" 
#include "pch.h"
#include "Texture.h"

namespace dae {
	Texture::Texture(SDL_Surface* pSurface, ID3D11Device* pDevice)
	{
		// Need a resource for the texture
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT result = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
		if (FAILED(result)) {
			assert(false);
		}

		// Get a view for the resource.
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		result = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
		if (FAILED(result)) {
			assert(false);
		}

		SDL_FreeSurface(pSurface);
	}

	Texture::~Texture()
	{
		if (m_pResource) {
			m_pResource->Release();
		}
	}
	Texture* Texture::LoadFromFile(const std::string& path, ID3D11Device* pDevice)
	{
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)

		SDL_Surface* texture = IMG_Load(path.c_str());
		return new Texture(texture, pDevice);
	}

	void Texture::CreateMipMaps(ID3D11DeviceContext* pdeviceContext)
	{
		pdeviceContext->GenerateMips(m_pSRV);
	}
}