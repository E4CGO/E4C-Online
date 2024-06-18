#pragma once

#include <wrl.h>
#include <d3d11.h>
#include "RenderContext.h"
#include "imgui.h"

class PostEffect
{
public:
	PostEffect(ID3D11Device* device);

	// デバッグGUI描画
	void DrawDebugGUI();

	// 開始処理
	void Begin(const RenderContext& rc);
	// 輝度抽出処理
	void LuminanceExtraction(const RenderContext& rc, ID3D11ShaderResourceView* colorMap);
	// ブルーム処理
	void Bloom(const RenderContext& rc, ID3D11ShaderResourceView* colorMap, ID3D11ShaderResourceView* luminanceMap);
	// 終了処理
	void End(const RenderContext& rc);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> fullscreenQuadVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> luminanceExtractionPS;

	struct CbPostEffect
	{
		float luminanceExtractionLowerEdge = 0.6f;
		float luminanceExtractionHigherEdge = 0.8f;
		float gaussianSigma = 1.0f;
		float bloomIntensity = 1.0f;
	};
	CbPostEffect cbPostEffect;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> bloomPS;
};
