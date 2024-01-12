#include "pch.h"
#include "BaseEffect.h"
#include <cassert>

namespace dae {
	BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		m_pEffect = LoadEffect(pDevice, assetFile);
		if (!m_pEffect)
			assert(false);
		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
		if (!m_pTechnique->IsValid()) {
			std::wcout << L"Technique not valid\n";
		}
		m_pMatWorldViewProjMatrix = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjMatrix->IsValid()) {
			std::wcout << L"The Effect Matrix Variable is not valid. \n";
		}
	}
	BaseEffect::~BaseEffect()
	{
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		if (m_pMatWorldViewProjMatrix) {
			m_pMatWorldViewProjMatrix->Release();
		}
		if (m_pInputLayout) {
			m_pInputLayout->Release();
		}
		if (m_pTechnique) {
			m_pTechnique->Release();
		}
		if (m_pEffect) {
			m_pEffect->Release();
		}
	}

	ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile(assetFile.c_str(), nullptr, nullptr, shaderFlags, 0,
			pDevice, &pEffect, &pErrorBlob);
		if (FAILED(result)) {
			if (pErrorBlob != nullptr) {
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++) {
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
			}
			else {
				std::wstringstream ss;
				ss << "EffectLoader: Failed to Create Effect from File! \nPath: " << assetFile;
				std::wcout << ss.str() << std::endl;
				return nullptr;
			}
		}

		return pEffect;
	}
	bool BaseEffect::CreateInputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t numElements)
	{
		D3DX11_PASS_DESC passDesc{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		const HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pInputLayout);
		if (FAILED(result)) {
			std::wcout << L"Input Layer could not be created.";
			return false;
		}
		return true;
	}

	void BaseEffect::SetWorldViewProjectionMatrix(const Matrix& m)
	{
		std::vector<float> data = m.GetData();
		m_pMatWorldViewProjMatrix->SetMatrix(&data[0]);
	}

	void BaseEffect::SetSamplerState(ID3D11Device* pDevice)
	{
		// Decide on the filter to use depending on the current state.
		D3D11_FILTER filter{};
		switch (m_filterMethod) {
		case (FilterMethod::Point):
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case (FilterMethod::Linear):
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case (FilterMethod::Anisotropic):
			filter = D3D11_FILTER_ANISOTROPIC;
			break;
		default:
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		}

		// Create the sampler description.
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = filter;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		// Create the Sampler State. Release the old one in case there is one.
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		HRESULT result = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
		if (FAILED(result)) {
			assert(false);
		}

		// Set the Sampler State
		ID3DX11EffectSamplerVariable* sampler = m_pEffect->GetVariableByName("samplerState")->AsSampler();
		sampler->SetSampler(0, m_pSamplerState);
	}

}