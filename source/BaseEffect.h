#pragma once
#include "Texture.h"
namespace dae {
	enum FilterMethod {
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

	class BaseEffect
	{
	public:
		BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~BaseEffect();

		BaseEffect(const BaseEffect&) = delete;
		BaseEffect(BaseEffect&&) noexcept = delete;
		BaseEffect& operator=(const BaseEffect&) = delete;
		BaseEffect& operator=(BaseEffect&&) noexcept = delete;

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		bool CreateInputLayout(ID3D11Device* pDevice, D3D11_INPUT_ELEMENT_DESC* vertexDesc, uint32_t numElements);

		ID3DX11Effect* GetEffect() { return m_pEffect; };
		ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; };
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; };

		void SetWorldViewProjectionMatrix(const Matrix& m);

		virtual bool CreateShaderResource(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile = "", const std::string& specularTextureFile = "", const std::string& glossinessTextureFile = "") = 0;
		virtual void CycleFilterMethod(ID3D11Device* pDevice) = 0;

	protected:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjMatrix{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };

		ID3D11SamplerState* m_pSamplerState{ nullptr };
		FilterMethod m_filterMethod{ FilterMethod::Point };

		void SetSamplerState(ID3D11Device* pDevice);
	};
}


