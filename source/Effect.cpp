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
}
