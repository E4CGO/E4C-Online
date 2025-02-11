//! @file RenderContext.h
//! @note

#ifndef __INCLUDE_RENDER_CONTEXT_H__
#define __INCLUDE_RENDER_CONTEXT_H__

#include <d3d12.h>

#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Rendering/RenderState.h"
#include "TAKOEngine/Rendering/MyRender.h"

#pragma region ShaderData(DX11)
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
#pragma region Shader Information
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
	DirectX::XMFLOAT2	textureSize = {};	// 暈すテクスチャのサイズ
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

//影情報
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
#pragma endregion

// ポストエフェクトの最終パス用情報
struct FinalpassDataDX12
{
	//ブルームテクスチャ
	const Descriptor* bloomTexture = nullptr;
};

// スカイボックス情報
struct SkydomeData
{
	//ブルームテクスチャ
	const Descriptor* skyTexture = nullptr;
};

//影情報
struct ShadowMapDataDX12
{
	const Descriptor* shadow_srv_descriptor = nullptr;
	const Descriptor* shadow_sampler_descriptor = nullptr;
};

// パーティクル情報
struct ParticleData
{
	// コンスタントバッファ
	const Descriptor* cbv_descriptor = nullptr;

	// パーティクル情報
	float elapsedTime = 0; //経過時間
	float deltaTime = 0;   //フレーム経過時間
	int emitCount = 0;     //出現させる個数
	int emitIndex = 0;     //現在の出現待ち

	DirectX::XMFLOAT4 startColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 endColor = { 0.0f, 1.0f, 0.0f, 1.0f };

	float scale = 0.3f;
	float lifetime = 1.0f;

	//ノイズ
	float noiseSpeed = 0.4f;
	float noisePower = 100.0f;
};

// ヒットパーティクル情報
struct HitParticleData
{
	// コンスタントバッファ
	const Descriptor* cbv_descriptor = nullptr;
	const Descriptor* srv_descriptor = nullptr;

	D3D12_VERTEX_BUFFER_VIEW d3d_vbv = {};
	int maxParticle = 0;
};

// 草情報
struct GrassData
{
	const Descriptor* grass_srv_descriptor = nullptr;
	const Descriptor* grass_srv_distortion_descriptor = nullptr;
	const Descriptor* grass_srv_density_descriptor = nullptr; // 草の生え具合
	DirectX::XMFLOAT4 position = {};
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	const Camera* camera;

	float timerGlobal;
	float timerTick;

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

// レンダーコンテキスト(DX12)
struct RenderContextDX12
{
	ID3D12GraphicsCommandList* d3d_command_list = nullptr;
	const Descriptor* scene_cbv_descriptor = nullptr;
	
	DirectX::XMFLOAT4X4	view;
	DirectX::XMFLOAT4X4	projection;
	DirectX::XMFLOAT4	light_direction;

	// スプライトシェーダー情報
	LuminanceExtractionData	luminanceExtractionData; //	高輝度抽出用情報 
	GaussianFilterData		gaussianFilterData;		 //	ガウスフィルター情報
	ColorGradingData		colorGradingData;		 //	色調補正情報
	FinalpassDataDX12		finalpassnData;			 //	最終パス情報

	// スカイボックス情報
	SkydomeData             skydomeData;

	// パーティクル情報
	ParticleData            particleData;
	HitParticleData         hitParticleData;

	// 影情報
	ShadowMapDataDX12       shadowMap;

	// 板のコンスタントバッファ
	const Descriptor* plane_cbv_descriptor = nullptr;

	// 草の情報
	GrassData  grassData;
};

#endif // !__INCLUDE_RENDER_CONTEXT_H__