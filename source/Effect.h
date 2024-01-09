#pragma once
#include "Texture.h"

namespace dae {
	enum FilterMethod{
		Point,
		Linear,
		Anisotropic
	};

	inline const char* ToString(FilterMethod fm)
	{
		switch (fm)
		{
		case Point:			return "Point";
		case Linear:		return "Linear";
		case Anisotropic:	return "Anisotropic";
		default:			return "Unknown";
		}
	}

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

		bool CreateShaderResource();

		ID3DX11Effect* GetEffect() { return m_pEffect; };
		ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; };
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; };

		void SetMatrix(Matrix m);
		void SetDiffuseMap(Texture* pDiffuseTexture);

		void CycleFilterMethod(ID3D11Device* pDevice);

	private:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjMatrix{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };
		ID3D11SamplerState* m_pSamplerState{ nullptr };

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

		FilterMethod m_filterMethod{ FilterMethod::Point };
		void SetSamplerState(ID3D11Device* pDevice);
	};
};

