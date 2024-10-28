//! @file PrimitiveRender.h
//! @note 

#ifndef __GRAPHICS_PrimitiveRender_SHADER_H__
#define __GRAPHICS_PrimitiveRender_SHADER_H__

#include "SpriteShader.h"
/**************************************************************************//**
    @class  PrimitiveRender  
    @brief   PrimitiveRenderのパラーメータ設定クラス 
    @par    [説明]
*//***************************************************************************/
class PrimitiveRender:public SpriteShader
{
public:
	//!コンストラクタ
	PrimitiveRender(ID3D11Device* device);
	~PrimitiveRender() override = default;

	//!頂点追加
	void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT2& texcoord);

	//!描画実行
	void Render(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection,
		D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);

private:
	static const UINT VertexCapacity = 1024;

	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		color;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2   texcoord;
	};
	std::vector<Vertex>		vertices;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	D3D11_TEXTURE2D_DESC textureDesc = {};
};
#endif // !__GRAPHICS_PHONG_SHADER_H__
