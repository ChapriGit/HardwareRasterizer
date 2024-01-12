#pragma once
#include "VehicleEffect.h"

namespace dae {
	struct Vertex_PosCol {
		Vector3 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
	};
	struct Vertex {
		Vector3 position{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
	};

	class TriangleMesh
	{
	public:
		TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, BaseEffect* pEffect, const std::string& objFilee);
		~TriangleMesh();

		TriangleMesh(const TriangleMesh&) = delete;
		TriangleMesh(TriangleMesh&&) noexcept = delete;
		TriangleMesh& operator=(const TriangleMesh&) = delete;
		TriangleMesh& operator=(TriangleMesh&&) noexcept = delete;

		void InitializeTextures(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string& normalTextureFile = "", const std::string& specularTextureFile = "", const std::string& glossinessTextureFile = "");

		void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix, const Vector3& cameraPosition);
		void CycleFilterMethod(ID3D11Device* pContext);

		void RotateY(float yaw) {
			m_worldMatrix = Matrix::CreateRotationY(yaw);
		}

	private:
		BaseEffect* m_pEffect{ nullptr };
		ID3D11Buffer* m_pVertexBuffer{ nullptr };
		ID3D11Buffer* m_pIndexBuffer{ nullptr };
		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjMatrix{ nullptr };
		uint32_t m_NumIndices{};

		Matrix m_worldMatrix{ {1, 0, 0, 0},
							{0, 1, 0, 0},
							{0, 0, 1, 0},
							{0, 0, 0, 1} };

		std::vector<Vertex> m_vertices{};
		std::vector<uint32_t> m_indices{};
	};
}

