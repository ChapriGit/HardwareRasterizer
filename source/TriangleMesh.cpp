#include "pch.h"
#include "TriangleMesh.h"
#include "Util.h"

namespace dae {
	TriangleMesh::TriangleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const std::string& objFile, const std::string& diffuseTextureFile)
	{
		// Create data for mesh
		//std::vector<Vertex_PosCol> vertices{ {{-3, 3, -2}, {}, {0, 0}}, {{0, 3, -2}, {}, {.5f, 0}} ,
		//	{ {3, 3, -2}, {}, {1, 0} }, { {-3, 0, -2}, {}, {0, .5} },
		//	{ {0, 0, -2}, {}, {.5, .5} }, { {3, 0, -2}, {}, {1, .5} }, 
		//	{ {-3, -3, -2}, {}, {0, 1} }, { {0, -3, -2}, {}, {.5, 1} },
		//	{ {3, -3, -2}, {}, {1, 1} } };
		//std::vector<uint32_t> indices{ 3, 0, 4, 1, 5, 2, 2, 6, 6, 3, 7, 4, 8, 5 };
		Util::ParseOBJ(objFile, m_vertices, m_indices);

		// Create Effect
		m_pEffect = new Effect(pDevice, L"./Resources/PosCol3D.fx");

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 3 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 4 bytes per element -> 12 bytes
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;		// 12 bytes
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;		// 8 bytes - Only float2
		vertexDesc[2].AlignedByteOffset = 24;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(m_vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result)) {
			return;
		}

		// Create Input Layout
		if (!m_pEffect->CreateInputLayout(pDevice, vertexDesc, numElements)) {
			return;
		};

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

		// Create Texture
		m_pDiffuseTexture = Texture::LoadFromFile(diffuseTextureFile, pDevice);
		m_pDiffuseTexture->CreateMipMaps(pDeviceContext);
		m_pEffect->CreateShaderResource();
		m_pEffect->SetDiffuseMap(m_pDiffuseTexture);
	}

	TriangleMesh::~TriangleMesh()
	{
		if (m_pIndexBuffer) {
			m_pIndexBuffer->Release();
		}
		if (m_pVertexBuffer) {
			m_pVertexBuffer->Release();
		}
		
		delete m_pEffect;
		delete m_pDiffuseTexture;
	}

	void TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext, Matrix viewProjectionMatrix)
	{
		// Set primitive topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Set Input Layout
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		// Set Vertex Buffer
		constexpr UINT stride = sizeof(Vertex_PosCol);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// Set Index Buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the WorldViewProjection matrix
		Matrix worldViewProjectionMatrix = m_worldMatrix * viewProjectionMatrix;
		m_pEffect->SetMatrix(worldViewProjectionMatrix);

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