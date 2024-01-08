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
		bool CreateInputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t numElements);

		ID3DX11Effect* GetEffect() { return m_pEffect; };
		ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; };
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; };

		void SetMatrix(Matrix m);

	private:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjMatrix{ nullptr };
		ID3D11InputLayout* m_pInputLayout;
	};
};

