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
	}

	bool FireEffect::CreateShaderResource() {
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