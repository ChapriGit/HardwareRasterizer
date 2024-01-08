#pragma once
#include "Effect.h"

namespace dae {
	struct Vertex_PosCol {
		Vector3 position{};
		ColorRGB color{};
		Vector2 uv{};
	};

	class TriangleMesh
	{
	public:
		TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile);
		~TriangleMesh();

		TriangleMesh(const TriangleMesh&) = delete;
		TriangleMesh(TriangleMesh&&) noexcept = delete;
		TriangleMesh& operator=(const TriangleMesh&) = delete;
		TriangleMesh& operator=(TriangleMesh&&) noexcept = delete;

		void Render(ID3D11DeviceContext* pDeviceContext, Matrix viewProjectionMatrix);

	private:
		Effect* m_pEffect{ nullptr };
		ID3D11Buffer* m_pVertexBuffer{ nullptr };
		ID3D11Buffer* m_pIndexBuffer{ nullptr };
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjMatrix{ nullptr };
		uint32_t m_NumIndices{};

		Matrix m_worldMatrix{ {1, 0, 0, 0},
							{0, 1, 0, 0},
							{0, 0, 1, 0},
							{0, 0, 0, 1} };
		Texture* m_pDiffuseTexture{ nullptr };
	};
}

