#include "pch.h"
#include "VehicleEffect.h"
#include <cassert>

namespace dae {
	VehicleEffect::VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile) : BaseEffect(pDevice, assetFile)
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
	VehicleEffect::~VehicleEffect()
	{
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

		delete m_pDiffuseTexture;
		delete m_pNormalTexture;
		delete m_pSpecularTexture;
		delete m_pGlossinessTexture;
	}

	bool VehicleEffect::CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile, const std::string& specularTextureFile, const std::string& glossinessTextureFile) {
		m_pDiffuseTexture = Texture::LoadFromFile(diffuseTextureFile, pDevice);
		m_pDiffuseTexture->CreateMipMaps(pDeviceContext);
		m_pNormalTexture = Texture::LoadFromFile(normalTextureFile, pDevice);
		m_pNormalTexture->CreateMipMaps(pDeviceContext);
		m_pSpecularTexture = Texture::LoadFromFile(specularTextureFile, pDevice);
		m_pSpecularTexture->CreateMipMaps(pDeviceContext);
		m_pGlossinessTexture = Texture::LoadFromFile(glossinessTextureFile, pDevice);
		m_pGlossinessTexture->CreateMipMaps(pDeviceContext);
	
		SetDiffuseMap();
		SetNormalMap();
		SetSpecularMap();
		SetGlossinessMap();

		return true;
	}
	void VehicleEffect::SetWorldMatrix(const Matrix& m)
	{
		std::vector<float> data = m.GetData();
		m_pMatWorldMatrix->SetMatrix(&data[0]);
	}
	void VehicleEffect::SetCameraOrigin(const Vector3& origin)
	{
		std::vector<float> data = {origin.x, origin.y, origin.z};
		m_pVecCameraOrigin->SetFloatVector(&data[0]);
	}
	void VehicleEffect::SetDiffuseMap()
	{
		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid()) {
			std::wcout << L"Diffuse Map Variable is not valid.";
			assert(false);
		}

		if (m_pDiffuseMapVariable) {
			m_pDiffuseMapVariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
		}
	}
	void VehicleEffect::SetNormalMap()
	{
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid()) {
			std::wcout << L"Normal Map Variable is not valid.";
			assert(false);
		}

		if (m_pNormalMapVariable) {
			m_pNormalMapVariable->SetResource(m_pNormalTexture->GetShaderResourceView());
		}
	}
	void VehicleEffect::SetSpecularMap()
	{
		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid()) {
			std::wcout << L"Specular Map Variable is not valid.";
			assert(false);
		}
		if (m_pSpecularMapVariable) {
			m_pSpecularMapVariable->SetResource(m_pSpecularTexture->GetShaderResourceView());
		}
	}
	void VehicleEffect::SetGlossinessMap()
	{
		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid()) {
			std::wcout << L"Glossiness Map Variable is not valid.";
			assert(false);
		}

		if (m_pGlossinessMapVariable) {
			m_pGlossinessMapVariable->SetResource(m_pGlossinessTexture->GetShaderResourceView());
		}
	}
	void VehicleEffect::CycleFilterMethod(ID3D11Device* pDevice)
	{
		m_filterMethod = static_cast<FilterMethod>((m_filterMethod + 1));
		if (m_filterMethod == 3)
			m_filterMethod = FilterMethod::Point;
		std::cout << "\n\nFILTER METHOD : " << ToString(m_filterMethod) << std::endl;

		SetSamplerState(pDevice);
	}
	void VehicleEffect::ToggleNormalMap()
	{
		m_normalMapping = !m_normalMapping;
		ID3DX11EffectScalarVariable* normalMapping = m_pEffect->GetVariableByName("gNormalMappingEnabled")->AsScalar();
		normalMapping->SetBool(m_normalMapping);

		std::cout << "\nNormal Map Toggled - " << m_normalMapping << std::endl;
	}
}
