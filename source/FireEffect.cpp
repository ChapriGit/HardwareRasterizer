#include "pch.h"
#include "FireEffect.h"
#include <cassert>

namespace dae {
	FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile) : BaseEffect(pDevice, assetFile)
	{
		SetSamplerState(pDevice);
	}
	FireEffect::~FireEffect()
	{
		// Release any DirectX resources.
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->Release();
		}
		delete m_pDiffuseTexture;
	}

	bool FireEffect::CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile, const std::string& specularTextureFile, const std::string& glossinessTextureFile) {
		m_pDiffuseTexture = Texture::LoadFromFile(diffuseTextureFile, pDevice);
		m_pDiffuseTexture->CreateMipMaps(pDeviceContext);
		
		SetDiffuseMap();
		return true;
	}

	void FireEffect::SetDiffuseMap()
	{
		// Get the global variable related to the diffuse map.
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) {
			std::wcout << L"Diffuse Map Variable is not valid.";
			assert(false);
		}

		// Set it to the saved View.
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
		}
	}

	void FireEffect::CycleFilterMethod(ID3D11Device* pDevice)
	{
		m_filterMethod = static_cast<FilterMethod>((m_filterMethod + 1));
		if (m_filterMethod == 3)
			m_filterMethod = FilterMethod::Point;

		SetSamplerState(pDevice);
	}
}