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

		bool CreateShaderResource();
		void SetDiffuseMap(Texture* pDiffuseTexture);

		void CycleFilterMethod(ID3D11Device* pDevice);

	private:
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	};
}

