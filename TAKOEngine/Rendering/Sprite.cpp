#include "Sprite.h"
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"

// コンストラクタ
Sprite::Sprite(ID3D11Device* device)
	:Sprite(device, nullptr)
{
}

// コンストラクタ
Sprite::Sprite(ID3D11Device* device, const char* filename)
{
	HRESULT hr = S_OK;

	// 頂点バッファの生成
	{
		// 頂点バッファを作成するための設定オプション
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(Vertex) * 4; // 4頂点
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		// 頂点バッファオブオブジェクトの生成
		hr = device->CreateBuffer(&buffer_desc, nullptr, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 頂点シェーダー
	{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = GpuResourceUtils::LoadVertexShader(
			device,
			"Data/Shader/SpriteVS.cso",
			inputElementDesc,
			ARRAYSIZE(inputElementDesc),
			inputLayout.GetAddressOf(),
			vertexShader.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// ピクセルシェーダー
	{
		hr = GpuResourceUtils::LoadPixelShader(
			device,
			"Data/Shader/SpritePS.cso",
			pixelShader.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// テクスチャ生成
	if (filename != nullptr)
	{
		// テクスチャファイル読み込み
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::LoadTexture(
			device,
			filename,
			shaderResourceView.GetAddressOf(),
			&desc
		);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureSize = {
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height)
		};
	}
	else
	{
		// ダミーテクスチャ生成
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, shaderResourceView.GetAddressOf(), &desc);

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		textureSize = {
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height)
		};
	}
}

void Sprite::Update(
	float dx, float dy,
	float dw, float dh,
	float sx, float sy,
	float sw, float sh,
	float angle,
	float r, float g, float b, float a) const
{
	ID3D11DeviceContext* immediate_context = Graphics::Instance().GetDeviceContext();
	{
		// スプライトを構成する４頂点のスクリーン座標を計算する
		DirectX::XMFLOAT2 positions[] = {
			DirectX::XMFLOAT2(dx,      dy),			// 左上
			DirectX::XMFLOAT2(dx + dw, dy),			// 右上
			DirectX::XMFLOAT2(dx,      dy + dh),	// 左下
			DirectX::XMFLOAT2(dx + dw, dy + dh),	// 右下
		};

		// スプライトを構成する４頂点のテクスチャ座標を計算する
		DirectX::XMFLOAT2 texcoords[] = {
			DirectX::XMFLOAT2(sx,      sy),			// 左上
			DirectX::XMFLOAT2(sx + sw, sy),			// 右上
			DirectX::XMFLOAT2(sx,      sy + sh),	// 左下
			DirectX::XMFLOAT2(sx + sw, sy + sh),	// 右下
		};

		// スプライトの中心で回転させるために４頂点の中心位置が
		// 原点(0, 0)になるように一旦頂点を移動させる。
		float mx = dx + dw * 0.5f;
		float my = dy + dh * 0.5f;
		for (auto& p : positions)
		{
			p.x -= mx;
			p.y -= my;
		}

		// 頂点を回転させる
		float theta = angle;	// 角度をラジアン(θ)に変換
		//float theta = DirectX::XMConvertToRadians(angle);	// 角度をラジアン(θ)に変換
		float c = cosf(theta);
		float s = sinf(theta);
		for (auto& p : positions)
		{
			DirectX::XMFLOAT2 r = p;
			p.x = c * r.x + -s * r.y;
			p.y = s * r.x + c * r.y;
		}

		// 回転のために移動させた頂点を元の位置に戻す
		for (auto& p : positions)
		{
			p.x += mx;
			p.y += my;
		}

		// 現在設定されているビューポートからスクリーンサイズを取得する。
		D3D11_VIEWPORT viewport;
		UINT numViewports = 1;
		immediate_context->RSGetViewports(&numViewports, &viewport);
		float screen_width = viewport.Width;
		float screen_height = viewport.Height;

		// スクリーン座標系からNDC座標系へ変換する。
		for (auto& p : positions)
		{
			p.x = 2.0f * p.x / screen_width - 1.0f;
			p.y = 1.0f - 2.0f * p.y / screen_height;
		}

		// 頂点バッファの内容の編集を開始する。
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		HRESULT hr = immediate_context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		// pDataを編集することで頂点データの内容を書き換えることができる。
		Vertex* v = static_cast<Vertex*>(mappedBuffer.pData);
		for (int i = 0; i < 4; ++i)
		{
			v[i].position.x = positions[i].x;
			v[i].position.y = positions[i].y;
			v[i].position.z = 0.0f;

			v[i].color.x = r;
			v[i].color.y = g;
			v[i].color.z = b;
			v[i].color.w = a;

			v[i].texcoord.x = texcoords[i].x / textureSize.x;
			v[i].texcoord.y = texcoords[i].y / textureSize.y;
		}

		// 頂点バッファの内容の編集を終了する。
		immediate_context->Unmap(vertexBuffer.Get(), 0);
	}
}

void Sprite::Render(
	ID3D11DeviceContext* dc,
	float dx, float dy,					// 位置
	float dz,							// 奧行
	float dw, float dh,					// サイズ
	float sx, float sy,					// 画像切り抜き位置
	float sw, float sh,					// 画像切り抜きサイズ
	float angle,						// 角度
	float r, float g, float b, float a	// 色
) const
{
	// 頂点座標
	DirectX::XMFLOAT2 positions[] = {
		DirectX::XMFLOAT2(dx, dy),				// 左上
		DirectX::XMFLOAT2(dx + dw, dy),			// 右上
		DirectX::XMFLOAT2(dx, dy + dh),			// 左下
		DirectX::XMFLOAT2(dx + dw, dy + dh),	// 右下
	};

	// テクスチャ座標
	DirectX::XMFLOAT2 texcorrds[] = {
		DirectX::XMFLOAT2(sx, sy),	// 左上
		DirectX::XMFLOAT2(sx + sw, sy),	// 右上
		DirectX::XMFLOAT2(sx, sy + sh),	// 左下
		DirectX::XMFLOAT2(sx + sw, sy + sh),	// 右下
	};

	// スプライトの中心で回転させる為に4頂点の中心位置が原点になるようにいったん頂点を移動させる
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	for (auto& p : positions)
	{
		p.x -= mx;
		p.y -= my;
	}

	// 頂点を回転させる
	float theta = angle;
	//float theta = DirectX::XMConvertToRadians(angle);
	float c = cosf(theta);
	float s = sinf(theta);
	for (auto& p : positions)
	{
		DirectX::XMFLOAT2 r = p;
		p.x = c * r.x - s * r.y;
		p.y = s * r.x + c * r.y;
	}

	// 元の位置に戻す
	for (auto& p : positions)
	{
		p.x += mx;
		p.y += my;
	}

	// 現在設定されているビューポートからスクリーンサイズを取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);
	float screenWidth = viewport.Width;
	float screenHeight = viewport.Height;

	// スクリーン座標からNDC座標系へ変換
	for (DirectX::XMFLOAT2& p : positions)
	{
		p.x = 2.0f * p.x / screenWidth - 1.0f;
		p.y = 1.0f - 2.0f * p.y / screenHeight;
	}

	// 頂点バッファの内容の編集を開始する
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 頂点バッファの内容を編集
	Vertex* v = static_cast<Vertex*>(mappedSubresource.pData);
	for (int i = 0; i < 4; ++i)
	{
		v[i].position.x = positions[i].x;
		v[i].position.y = positions[i].y;
		v[i].position.z = dz;					// 奥行

		v[i].color.x = r;
		v[i].color.y = g;
		v[i].color.z = b;
		v[i].color.w = a;

		v[i].texcoord.x = texcorrds[i].x / textureSize.x;
		v[i].texcoord.y = texcorrds[i].y / textureSize.y;
	}

	// 頂点バッファの内容の編集を終了する
	dc->Unmap(vertexBuffer.Get(), 0);

	// GPUに描画する為のデータを渡す
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetInputLayout(inputLayout.Get());
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	// レンダーステート設定
	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	dc->OMSetBlendState(T_GRAPHICS.GetRenderState()->GetBlendState(BlendState::Transparency), blend_factor, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(T_GRAPHICS.GetRenderState()->GetDepthStencilState(DepthState::TestAndWrite), 0);
	dc->RSSetState(T_GRAPHICS.GetRenderState()->GetRasterizerState(RasterizerState::SolidCullBack));

	// 描画
	dc->Draw(4, 0);

	dc->VSSetShader(nullptr, nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
	dc->IASetInputLayout(nullptr);
}

void Sprite::Render(
	ID3D11DeviceContext* dc,
	float dx, float dy,					// 位置
	float dz,							// 奧行
	float dw, float dh,					// サイズ
	float angle,						// 角度
	float r, float g, float b, float a	// 色
) const
{
	Render(dc, dx, dy, dz, dw, dh, 0, 0, textureSize.x, textureSize.y, angle, r, g, b, a);
}

void Sprite::Render(
	ID3D11DeviceContext* dc,
	float dx, float dy,					// 位置
	float dz,							// 奧行
	float dw, float dh					// サイズ
) const
{
	Render(dc, dx, dy, dz, dw, dh, 0, 0, textureSize.x, textureSize.y, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

// シェーダーリソースビューの設定
void Sprite::SetShaderResourceView(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight)
{
	shaderResourceView = srv;
	textureSize = {
		static_cast<float>(texWidth),
		static_cast<float>(texHeight)
	};
}