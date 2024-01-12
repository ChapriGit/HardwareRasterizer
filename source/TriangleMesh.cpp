#include "pch.h"
#include "TriangleMesh.h"
#include "Util.h"

namespace dae {
	TriangleMesh::TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, BaseEffect* pEffect, const std::string& objFile)
	{
		// Create data for mesh
		//std::vector<Vertex_PosCol> vertices{ {{-3, 3, -2}, {}, {0, 0}}, {{0, 3, -2}, {}, {.5f, 0}} ,
		//	{ {3, 3, -2}, {}, {1, 0} }, { {-3, 0, -2}, {}, {0, .5} },
		//	{ {0, 0, -2}, {}, {.5, .5} }, { {3, 0, -2}, {}, {1, .5} }, 
		//	{ {-3, -3, -2}, {}, {0, 1} }, { {0, -3, -2}, {}, {.5, 1} },
		//	{ {3, -3, -2}, {}, {1, 1} } };
		//std::vector<uint32_t> indices{ 3, 0, 4, 1, 5, 2, 2, 6, 6, 3, 7, 4, 8, 5 };
		Util::ParseOBJ(objFile, m_vertices, m_indices);
		std::cout << "Succesfully parsed object." << std::endl;

		// Create Effect
		m_pEffect = pEffect;

		// Create Vertex Layout
		std::cout << "Creating Index and Vertex buffers... (0/4)" << std::endl;
		static constexpr uint32_t numElements{ 4 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 4 bytes per element -> 12 bytes
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "TEXCOORD";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;		// 8 bytes - Only float2
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "NORMAL";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 12 bytes
		vertexDesc[2].AlignedByteOffset = 20;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[3].SemanticName = "TANGENT";
		vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 12 bytes
		vertexDesc[3].AlignedByteOffset = 32;
		vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		std::cout << "Vertex Layout Created. (1/4)" << std::endl;

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(m_vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result)) {
			return;
		}

		std::cout << "Vertex Buffer Created. (2/4)" << std::endl;

		// Create Input Layout
		if (!m_pEffect->CreateInputLayout(pDevice, vertexDesc, numElements)) {
			return;
		};
		std::cout << "Input Layout Created. (3/4)" << std::endl;


		// Create Index Buffer
		m_NumIndices = static_cast<uint32_t>(m_indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		initData.pSysMem = m_indices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;

		std::cout << "Index Buffer Created. (4/4)" << std::endl;
	}

	TriangleMesh::~TriangleMesh()
	{
		if (m_pMatWorldViewProjMatrix) {
			m_pMatWorldViewProjMatrix->Release();
		}
		if (m_pIndexBuffer) {
			m_pIndexBuffer->Release();
		}
		if (m_pVertexBuffer) {
			m_pVertexBuffer->Release();
		}
		
		delete m_pEffect;
	}

	void TriangleMesh::InitializeTextures(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& diffuseTextureFile, const std::string & normalTextureFile, const std::string& specularTextureFile, const std::string& glossinessTextureFile)
	{
		m_pEffect->CreateShaderResource(pDevice, pDeviceContext, diffuseTextureFile, normalTextureFile, specularTextureFile, glossinessTextureFile);
	}

	void TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext, const Matrix& viewProjectionMatrix, const Vector3& cameraPosition)
	{
		// Set primitive topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set Input Layout
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		// Set Vertex Buffer
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// Set Index Buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the WorldViewProjection matrix
		Matrix worldViewProjectionMatrix = m_worldMatrix * viewProjectionMatrix;
		m_pEffect->SetWorldViewProjectionMatrix(worldViewProjectionMatrix);

		if (dynamic_cast<VehicleEffect*>(m_pEffect)) {
			// Set the World Matrix
			static_cast<VehicleEffect*>(m_pEffect)->SetWorldMatrix(m_worldMatrix);

			// Set Camera Origin
			static_cast<VehicleEffect*>(m_pEffect)->SetCameraOrigin(cameraPosition);
		}

		// Draw
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p) {
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

	void TriangleMesh::CycleFilterMethod(ID3D11Device* pContext)
	{
		m_pEffect->CycleFilterMethod(pContext);
	}

}