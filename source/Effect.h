#pragma once
#include "Texture.h"
#include "BaseEffect.h"

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

	class Effect : public BaseEffect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		Effect(const Effect&) = delete;
		Effect(Effect&&) noexcept = delete;
		Effect& operator=(const Effect&) = delete;
		Effect& operator=(Effect&&) noexcept = delete;

		bool CreateShaderResource();

		void SetWorldMatrix(const Matrix& m);
		void SetCameraOrigin(const Vector3& origin);

		void SetDiffuseMap(Texture* pDiffuseTexture);
		void SetNormalMap(Texture* pNormalTexture);
		void SetSpecularMap(Texture* pSpecularTexture);
		void SetGlossinessMap(Texture* pGlossinessTexture);

		void CycleFilterMethod(ID3D11Device* pDevice);

	private:
		ID3DX11EffectMatrixVariable* m_pMatWorldMatrix{ nullptr };
		ID3DX11EffectVectorVariable* m_pVecCameraOrigin{ nullptr };
		ID3D11SamplerState* m_pSamplerState{ nullptr };

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };

		FilterMethod m_filterMethod{ FilterMethod::Point };
		void SetSamplerState(ID3D11Device* pDevice);
	};
};

