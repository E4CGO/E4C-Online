#pragma once

#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Rendering/RenderState.h"
#include "TAKOEngine/Rendering/MyRender.h"

//	UVスクロール情報
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UVスクロール値
};

// マスクデータ
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float dissolveThreshold;
	float edgeThreshold; 			// 縁の閾値
	DirectX::XMFLOAT4 edgeColor;	// 縁の色
};

struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
};

//Deferred Rendering用SRV
struct DeferredData
{
	ID3D11ShaderResourceView* normal;
	ID3D11ShaderResourceView* position;
};

//TODO : ShaderData Set
#pragma region シェーダー情報
struct Default
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(0, 0, 0, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

struct Phong
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(1, 1, 1, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

struct Toon
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(1, 1, 1, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

#pragma endregion

struct ShaderData
{
	Default defaultShader;
	Phong   phongShader;
	Toon    toonShader;
};

// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4 position;		// 座標
	DirectX::XMFLOAT4 color;		// 色
	float range;					// 範囲
	DirectX::XMFLOAT3 dummy;
};

// 点光源の最大数
static constexpr int PointLightMax = 8;
// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	float				innerCorn; 	// インナー角度範囲
	float				outerCorn; 	// アウター角度範囲
	float				dummy;
};

// スポットライトの最大数
static	constexpr	int	SpotLightMax = 8;

// 色調補正情報
struct ColorGradingData
{
	float	hueShift = 0;	// 色相調整
	float	saturation = 1;	// 彩度調整
	float	brightness = 1;	// 明度調整
	float	dummy;
};

// ガウスフィルター計算情報
struct GaussianFilterData
{
	int					kernelSize = 8;		// カーネルサイズ
	float				deviation = 10.0f;	// 標準偏差
	DirectX::XMFLOAT2	textureSize;			// 暈すテクスチャのサイズ
};

// ガウスフィルターの最大カーネルサイズ
static const int MaxKernelSize = 16;

// 高輝度抽出用情報
struct LuminanceExtractionData
{
	float				threshold = 0.5f;	// 閾値
	float				intensity = 1.0f;	// ブルームの強度
	DirectX::XMFLOAT2	dummy2;
};

// ポストエフェクトの最終パス用情報
struct FinalpassnData
{
	//	ブルームテクスチャ
	ID3D11ShaderResourceView* bloomTexture;
};

struct ShadowMapData
{
	//カメラ情報
	DirectX::XMFLOAT4X4 view = {};
	DirectX::XMFLOAT4X4 projection = {};

	//シャドウマップ用深度ステンシルバッファ
	ID3D11ShaderResourceView* shadowMap[myRenderer::NUM_SHADOW_MAP] = {};

	//ライトビュープロジェクション行列
	DirectX::XMFLOAT4X4 lightViewProjection[myRenderer::NUM_SHADOW_MAP] = {};

	//深度比較用のオフセット値
	float shadowBias[myRenderer::NUM_SHADOW_MAP] = { 0.001f, 0.002f, 0.004f, 0.01f }; //深度比較用のオフセット値
	
	//影の色
	DirectX::XMFLOAT3 shadowColor = { 0.5f,0.5f,0.5f };
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	const Camera* camera;

	//	スクロールデータ
	UVScrollData			uvScrollData;

	// マスクデータ
	MaskData				maskData;

	//	ライト情報
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData			pointLightData[PointLightMax];	// 点光源情報
	SpotLightData			spotLightData[SpotLightMax];	// スポットライト情報
	int						pointLightCount = 0;			// 点光源数
	int						spotLightCount = 0;				// スポットライト数

	ColorGradingData		colorGradingData;				//	色調補正情報
	GaussianFilterData		gaussianFilterData;				//	ガウスフィルター情報
	LuminanceExtractionData	luminanceExtractionData;		//	高輝度抽出用情報
	FinalpassnData			finalpassnData;					//	最終パス情報
	ShadowMapData           shadowMapData;                  //  シャドウマップ情報

	//モデルシェーダー情報
	ShaderData shaderData;

	//Deferred Rendering用SRV
	DeferredData deferredData;
};