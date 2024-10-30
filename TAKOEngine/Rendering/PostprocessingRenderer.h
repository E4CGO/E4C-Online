//! @file PostprocessingRenderer.h
//! @note 

#ifndef __GRAHICS_POSTPROCESSING_RENDERER_H__
#define __GRAHICS_POSTPROCESSING_RENDERER_H__

#include "Graphics.h"
#include "Sprite.h"

//*******************************************************
// @class PostprocessingRenderer
// @brief ポストプロセス用のレンダラー
// @par   DX11 
//*******************************************************
class PostprocessingRenderer
{
	// ブルーム情報
	struct BloomData
	{
		//	高輝度抽出用情報
		LuminanceExtractionData	luminanceExtractionData;
		//	ガウスブラー用情報
		GaussianFilterData		gaussianFilterData;
	};

public:
	PostprocessingRenderer();
	~PostprocessingRenderer();

	//	描画
	void Render(ID3D11DeviceContext* deviceContext);

	//	デバッグ情報の表示
	void DrawDebugGUI();

	void SetBloom(const BloomData& bd) {
		bloomData.gaussianFilterData.deviation = bd.gaussianFilterData.deviation;
		bloomData.gaussianFilterData.kernelSize = bd.gaussianFilterData.kernelSize;
		bloomData.luminanceExtractionData.intensity = bd.luminanceExtractionData.intensity;
		bloomData.luminanceExtractionData.threshold = bd.luminanceExtractionData.threshold;
	}
	void SetColoGrading(const ColorGradingData& cg) { colorGradingData = cg; }
private:
	// 描画用スプライト
	std::unique_ptr<Sprite>			renderSprite;

	// ブルームデータ
	BloomData						bloomData;

	// 色調補正データ
	ColorGradingData				colorGradingData;
};

//*******************************************************
// @class PostprocessingRenderer
// @brief ポストプロセス用のレンダラー
// @par   DX12 
//*******************************************************
class PostprocessingRendererDX12
{
	// ブルーム情報
	struct BloomData
	{
		//	高輝度抽出用情報
		LuminanceExtractionData	luminanceExtractionData;
		
		//	ガウスブラー用情報
		GaussianFilterData		gaussianFilterData;
	};

	// テクスチャ
	enum class RenderTarget
	{
		Luminance,
		GaussianBlur,
		Finalpass,

		EnumCount
	};

public:
	PostprocessingRendererDX12();
	~PostprocessingRendererDX12();

	void Render(FrameBufferManager* framBuffer);

	//	デバッグ情報の表示
	void DrawDebugGUI();

	void SetBloom(const BloomData& bd) 
	{
		bloomData.gaussianFilterData.deviation = bd.gaussianFilterData.deviation;
		bloomData.gaussianFilterData.kernelSize = bd.gaussianFilterData.kernelSize;
		bloomData.luminanceExtractionData.intensity = bd.luminanceExtractionData.intensity;
		bloomData.luminanceExtractionData.threshold = bd.luminanceExtractionData.threshold;
	}
	
	void SetColoGrading(const ColorGradingData& cg) { colorGradingData = cg; }

private:
	// 描画用スプライト
	std::unique_ptr<SpriteDX12> 	renderSprite[static_cast<int>(RenderTarget::EnumCount)];

	// ブルームデータ
	BloomData						bloomData;

	// 色調補正データ
	ColorGradingData				colorGradingData;
};

#endif // !__GRAHICS_POSTPROCESSING_RENDERER_H__
