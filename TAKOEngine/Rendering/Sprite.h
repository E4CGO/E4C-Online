#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Sprite
{
public:
	Sprite(ID3D11Device* device);
	Sprite(ID3D11Device* device, const char* filename);

	const DirectX::XMFLOAT2 GetTextureSize() const { return textureSize; }

	// 頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	// バッファ更新
	virtual void Update(
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	// 描画実行
	void Render(
		ID3D11DeviceContext* dc,
		float dx, float dy,					// 位置
		float dz,							// 奧行
		float dw, float dh,					// サイズ
		float sx, float sy,					// 画像切り抜き位置
		float sw, float sh,					// 画像切り抜きサイズ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
	) const;

	// 描画実行 (テクスチャ切り抜き指定なし)
	void Render(
		ID3D11DeviceContext* dc,
		float dx, float dy,					// 位置
		float dz,							// 奧行
		float dw, float dh,					// サイズ
		float angle,						// 角度
		float r, float g, float b, float a	// 色
	) const;
	// 描画実行 (テクスチャ切り抜き指定なし色角度指定なし)
	void Render(
		ID3D11DeviceContext* dc,
		float dx, float dy,					// 位置
		float dz,							// 奧行
		float dw, float dh					// サイズ
	) const;

	// シェーダーリソースビューの設定
	void SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight);

	// 頂点バッファの取得
	const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }

	// シェーダーリソースビューの取得
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	DirectX::XMFLOAT2 textureSize = {};
};
