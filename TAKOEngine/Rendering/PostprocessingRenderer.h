#pragma once

#include "Graphics.h"
#include "Sprite.h"

// ポストプロセス用のレンダラー
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
