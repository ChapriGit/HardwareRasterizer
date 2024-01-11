#include "pch.h"
#include "Effect.h"
#include <cassert>

namespace dae {
	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) : BaseEffect(pDevice, assetFile)
	{
		m_pMatWorldMatrix = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pMatWorldMatrix->IsValid()) {
			std::wcout << L"The Effect Matrix Variable is not valid. \n";
		}
		m_pVecCameraOrigin = m_pEffect->GetVariableByName("gCameraPosition")->AsVector();
		if (!m_pVecCameraOrigin->IsValid()) {
			std::wcout << L"The Effect Matrix Variable is not valid. \n";
		}
		
		SetSamplerState(pDevice);
	}
	Effect::~Effect()
	{
		if (m_pSamplerState) {
			m_pSamplerState->Release();
		}
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->Release();
		}
		if (m_pNormalMapVariable) {
			m_pNormalMapVariable->Release();
		}
		if (m_pSpecularMapVariable) {
			m_pSpecularMapVariable->Release();
		}
		if (m_pGlossinessMapVariable) {
			m_pGlossinessMapVariable->Release();
		}
		if (m_pVecCameraOrigin) {
			m_pVecCameraOrigin->Release();
		}
		if (m_pMatWorldMatrix) {
			m_pMatWorldMatrix->Release();
		}
	}

	bool Effect::CreateShaderResource() {
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) {
			std::wcout << L"Diffuse Map Variable is not valid.";
			return false;
		}
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid()) {
			std::wcout << L"Normal Map Variable is not valid.";
			return false;
		}
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid()) {
			std::wcout << L"Specular Map Variable is not valid.";
			return false;
		}
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid()) {
			std::wcout << L"Glossiness Map Variable is not valid.";
			return false;
		}
		return true;
	}
	void Effect::SetWorldMatrix(const Matrix& m)
	{
		std::vector<float> data = m.GetData();
		m_pMatWorldMatrix->SetMatrix(&data[0]);
	}
	void Effect::SetCameraOrigin(const Vector3& origin)
	{
		std::vector<float> data = {origin.x, origin.y, origin.z};
		m_pVecCameraOrigin->SetFloatVector(&data[0]);
	}
	void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
	{
		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
		}
	}
	void Effect::SetNormalMap(Texture* pNormalTexture)
	{
		if (m_pNormalMapVariable) {
			m_pNormalMapVariable->SetResource(pNormalTexture->GetShaderResourceView());
		}
	}
	void Effect::SetSpecularMap(Texture* pSpecularTexture)
	{
		if (m_pSpecularMapVariable) {
			m_pSpecularMapVariable->SetResource(pSpecularTexture->GetShaderResourceView());
		}
	}
	void Effect::SetGlossinessMap(Texture* pGlossinessTexture)
	{
		if (m_pGlossinessMapVariable) {
			m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetShaderResourceView());
		}
	}
	void Effect::CycleFilterMethod(ID3D11Device* pDevice)
	{
		m_filterMethod = static_cast<FilterMethod>((m_filterMethod + 1));
		if (m_filterMethod == 3)
			m_filterMethod = FilterMethod::Point;
		std::cout << "\n\nFILTER METHOD : " << ToString(m_filterMethod) << std::endl;

		SetSamplerState(pDevice);

	}
	void Effect::SetSamplerState(ID3D11Device* pDevice)
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
