#pragma once

#include <memory>
#include <vector>

#include "TAKOEngine\Rendering\MyRender.h"
#include "TAKOEngine\Rendering\RenderContext.h"
#include "TAKOEngine\Rendering\FrameBuffer.h"
#include "TAKOEngine\Rendering\Model.h"
#include "TAKOEngine\Rendering\Light.h"

// 深度ステンシルバッファ
class DepthStencil
{
public:
	DepthStencil(UINT width, UINT height);
	~DepthStencil() {}

	// シェーダーリソースビュー取得
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return shaderResourceView; }

	// 深度ステンシルビュー取得
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return depthStencilView; }

	// テクスチャの情報取得
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }

	// テクスチャ幅取得
	inline int GetWidth() const { return texture2dDesc.Width; }

	// テクスチャ高さ取得
	inline int GetHeight() const { return texture2dDesc.Height; }

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		depthStencilView;
	D3D11_TEXTURE2D_DESC								texture2dDesc;
};

namespace myRenderer
{
	namespace shadow
	{
		class ShadowMapRender
		{
		public:
			//初期化
			void Initialize();

			//シャドウマップに描画するモデルを登録
			void ModelRegister(Model* model);

			//シャドウマップに使用するライトを取得
			void SetShadowLight(Light* light);

			//全削除
			void Clear();

			//描画
			void Render();

			//GUI
			void DrawDebugGUI();

			// シャドウマップ情報をRenderContextに積む
			ShadowMapData GetShadowMapData();

		private:
			std::unique_ptr<DepthStencil>	depthStencil[NUM_SHADOW_MAP];	// シャドウマップ用深度バッファ
			std::vector<Model*> m_models;
			Light* light = nullptr;

			//ライトビュープロジェクション行列
			DirectX::XMFLOAT4X4 lightViewProjection[NUM_SHADOW_MAP] = {};

			DirectX::XMFLOAT4X4 view = {};
			DirectX::XMFLOAT4X4 projection = {};

			//深度ステンシル設定用
			D3D11_TEXTURE2D_DESC texture2dDesc;
			UINT width = 1024;
		};

	}
}
 