#pragma once

namespace dae {
	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		Effect(const Effect&) = delete;
		Effect(Effect&&) noexcept = delete;
		Effect& operator=(const Effect&) = delete;
		Effect& operator=(Effect&&) noexcept = delete;

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

		ID3DX11Effect* getEffect() { return m_pEffect; };
		ID3DX11EffectTechnique* getTechnique() { return m_pTechnique; };

	private:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
	};
};

