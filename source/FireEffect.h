#pragma once
#include "BaseEffect.h"

namespace dae {
	class FireEffect : BaseEffect
	{
	public:
		FireEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~FireEffect();

		FireEffect(const FireEffect&) = delete;
		FireEffect(FireEffect&&) noexcept = delete;
		FireEffect& operator=(const FireEffect&) = delete;
		FireEffect& operator=(FireEffect&&) noexcept = delete;

		bool CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile = "", const std::string& specularTextureFile = "", const std::string& glossinessTextureFile = "");
		void SetDiffuseMap(Texture* pDiffuseTexture);

		void CycleFilterMethod(ID3D11Device* pDevice);

	private:
		Texture* m_pDiffuseTexture{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	};
}

