#include "pch.h"
#include "TriangleMesh.h"

namespace dae {
	TriangleMesh::TriangleMesh(ID3D11Device* pDevice)
	{
		// Create data for mesh
		std::vector<Vertex_PosCol> vertices{
			{{0.f, .5f, .5f}, {1.f, 0.f, 0.f}},
			{{.5f, -.5f, .5f}, {0.f, 0.f, 1.f}},
			{{-.5f, -.5f, .5f}, {0.f, 1.f, 0.f}} };
		std::vector<uint32_t> indices{ 0, 1, 2 };

		// Create Effect
		m_pEffect = new Effect(pDevice, L"./Resources/PosCol3D.fx");

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		// Create Vertex Buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result)) {
			return;
		}

		// Create Input Layout
		if (!m_pEffect->CreateInputLayout(pDevice, vertexDesc, numElements)) {
			return;
		};

		// Create Index Buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		initData.pSysMem = indices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result))
			return;
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
	}

	void TriangleMesh::Render(ID3D11DeviceContext* pDeviceContext)
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

		// Draw
		D3DX11_TECHNIQUE_DESC techDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p) {
			m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

}