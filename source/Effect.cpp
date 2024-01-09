#include "pch.h"
#include "Effect.h"
#include <cassert>

namespace dae {
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
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

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		HRESULT result = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
		if (FAILED(result)) {
			assert(false);
		}

		ID3DX11EffectSamplerVariable* sampler = m_pEffect->GetVariableByName("samplerState")->AsSampler();
		sampler->SetSampler(0, m_pSamplerState);
	}
	Effect::~Effect()
	{
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->Release();
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

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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
	bool Effect::CreateInputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t numElements)
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
	bool Effect::CreateShaderResource() {
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) {
			std::wcout << L"Diffuse Map Variable is not valid.";
			return false;
		}
		return true;
	}
	void Effect::SetMatrix(Matrix m)
	{
		std::vector<float> data = m.GetData();
		m_pMatWorldViewProjMatrix->SetMatrix(&data[0]);
	}
	void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
		}
	}
	void Effect::CycleFilterMethod(ID3D11Device* pDevice)
	{
		m_filterMethod = static_cast<FilterMethod>((m_filterMethod + 1));
		if (m_filterMethod == 3)
			m_filterMethod = FilterMethod::Point;
		std::cout << "\n\nFILTER METHOD : " << ToString(m_filterMethod) << std::endl;

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		switch (m_filterMethod) {
		case (FilterMethod::Point):
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case (FilterMethod::Linear):
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case (FilterMethod::Anisotropic):
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			break;
		default:
			break;
		}

		HRESULT result = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
		if (FAILED(result)) {
			std::wcout << L"Could not create new Sampler State.";
			assert(false);
		}

		ID3DX11EffectSamplerVariable* sampler = m_pEffect->GetVariableByName("samplerState")->AsSampler();
		if (!sampler->IsValid()) {
			std::wcout << L"Sampler State Variable is not valid.";
			assert(false);
		}
		sampler->SetSampler(0, m_pSamplerState);

	}
}
