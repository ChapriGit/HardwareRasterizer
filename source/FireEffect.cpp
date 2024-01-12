#include "pch.h"
#include "FireEffect.h"

namespace dae {
	FireEffect::FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile) : BaseEffect(pDevice, assetFile)
	{
		SetSamplerState(pDevice);
	}
	FireEffect::~FireEffect()
	{
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->Release();
		}
		delete m_pDiffuseTexture;
	}

	bool FireEffect::CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile, const std::string& specularTextureFile, const std::string& glossinessTextureFile) {
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) {
			std::wcout << L"Diffuse Map Variable is not valid.";
			return false;
		}
		return true;
	}

	void FireEffect::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
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