#pragma once
namespace dae {
	class Texture
	{
	public:
		~Texture();
		static Texture* LoadFromFile(const std::string& path, ID3D11Device* pDevice);

		ID3D11ShaderResourceView* GetShaderResourceView() { return m_pSRV; };
		void CreateMipMaps(ID3D11DeviceContext* pdeviceContext);

	private:
		Texture(SDL_Surface* pSurface, ID3D11Device* pDevice);
		ID3D11Texture2D* m_pResource{ nullptr };		// The resource itself.
		ID3D11ShaderResourceView* m_pSRV{ nullptr };	// The view for the texture. Describing how it should be used in the pipeline.
	};
}

