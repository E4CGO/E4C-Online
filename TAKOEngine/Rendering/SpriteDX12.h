//! @file SpriteDX12.h
//! @note DX12で画像を描画

#ifndef __INCLUDED_SPRITEDX12_H__
#define __INCLUDED_SPRITEDX12_H__

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Descriptor.h"
#include "FrameBufferTexture.h"

/**************************************************************************//**
		@class		SpriteDX12
		@brief		DX12で画像描画クラス
		@par		画像をローディングして画面に描画する
*//***************************************************************************/
class SpriteDX12
{
private:
	struct CbSpriteData
	{
		// 輝度シェーダー
		float threshold;	// 閾値
		float intensity;	// ブルームの強度
		DirectX::XMFLOAT2 dummy;

		// GaussianBlur
		DirectX::XMFLOAT4 weights[MaxKernelSize * MaxKernelSize];  // 重さ
		DirectX::XMFLOAT2 textureSize;   // 暈すテクスチャのサイズ
		float kernelSize;                // カーネルサイズ
		float dummy1;

		// ColorGrading
		float hueShift;	    // 色相調整
		float saturation;	// 彩度調整
		float brightness;	// 明度調整
		float dummy2;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

public:
	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>			d3d_vb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>			d3d_ib_resource;
		D3D12_VERTEX_BUFFER_VIEW						d3d_vbv;
		D3D12_INDEX_BUFFER_VIEW							d3d_ibv;
		Vertex* vertex_data = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource>			d3d_cbv_resource;
		const Descriptor* cbv_descriptor = nullptr;
		CbSpriteData* cb_scene_data = nullptr;
	};
	std::vector<FrameResource>	m_frame_resources;

	//! コンストラクタ
	SpriteDX12(UINT sprite_count, const char* filename = nullptr);
	SpriteDX12(UINT sprite_count, FrameBufferTexture* frameBuffer);

	//! デストラクタ
	~SpriteDX12();

	// 描画開始
	void Begin(const RenderContextDX12& rc);

	// 描画登録
	void Draw(
		float dx, float dy,
		float dw, float dh,
		float angle,
		float r, float g, float b, float a);

	// 描画登録
	void Draw(
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a,
		D3D12_VIEWPORT viewport);

	// 描画終了
	void End(ID3D12GraphicsCommandList* d3d_command_list);

	// テクスチャ幅取得
	int GetTextureWidth() const { return m_texture_width; }

	// テクスチャ高さ取得
	int GetTextureHeight() const { return m_texture_height; }

	// ディスクリプタ取得
	const Descriptor* GetDescriptor() { return m_srv_descriptor; }

	// スプライトカウント取得
	const int GetSpriteCount() { return m_sprite_count; }

private:
	// フレームリソース生成
	void CreateFrameResource();

	// フィルター値計算
	void CalcGaussianFilter(FrameResource& fram_resource, const GaussianFilterData& gaussianFilterData);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_d3d_texture;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_d3d_root_signature;
	const Descriptor* m_srv_descriptor = nullptr;
	int													m_texture_width = 0;
	int													m_texture_height = 0;
	int													m_sprite_index = 0;
	int													m_sprite_count = 0;
};

#endif  // __INCLUDED_SPRITEDX12_H__