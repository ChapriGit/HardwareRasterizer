#pragma once
#include "Effect.h"

namespace dae {
	struct Vertex_PosCol {
		Vector3 position{};
		ColorRGB color{};
	};

	class TriangleMesh
	{
	public:
		TriangleMesh(ID3D11Device* pDevice);
		~TriangleMesh();

		TriangleMesh(const TriangleMesh&) = delete;
		TriangleMesh(TriangleMesh&&) noexcept = delete;
		TriangleMesh& operator=(const TriangleMesh&) = delete;
		TriangleMesh& operator=(TriangleMesh&&) noexcept = delete;

		void Render(ID3D11DeviceContext* pDeviceContext);

	private:
		Effect* m_pEffect{ nullptr };
		ID3D11Buffer* m_pVertexBuffer{ nullptr };
		ID3D11Buffer* m_pIndexBuffer{ nullptr };
		uint32_t m_NumIndices{};
	};
}

