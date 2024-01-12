#pragma once
#include "Texture.h"
#include "BaseEffect.h"

namespace dae {
	class VehicleEffect : public BaseEffect
	{
	public:
		VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~VehicleEffect();

		VehicleEffect(const VehicleEffect&) = delete;
		VehicleEffect(VehicleEffect&&) noexcept = delete;
		VehicleEffect& operator=(const VehicleEffect&) = delete;
		VehicleEffect& operator=(VehicleEffect&&) noexcept = delete;

		bool CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile, const std::string& specularTextureFile, const std::string& glossinessTextureFile);

		void SetWorldMatrix(const Matrix& m);
		void SetCameraOrigin(const Vector3& origin);

		void CycleFilterMethod(ID3D11Device* pDevice);

	private:
		Texture* m_pDiffuseTexture{ nullptr };
		Texture* m_pNormalTexture{ nullptr };
		Texture* m_pSpecularTexture{ nullptr };
		Texture* m_pGlossinessTexture{ nullptr };

		ID3DX11EffectMatrixVariable* m_pMatWorldMatrix{ nullptr };
		ID3DX11EffectVectorVariable* m_pVecCameraOrigin{ nullptr };

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

		void SetDiffuseMap();
		void SetNormalMap();
		void SetSpecularMap();
		void SetGlossinessMap();
	};
};

