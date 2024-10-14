//! @file SpriteDX12.h
//! @note DX12で画像を描画

#ifndef __INCLUDED_SPRITEDX12_H__
#define __INCLUDED_SPRITEDX12_H__

#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "TAKOEngine/Rendering/Descriptor.h"

/**************************************************************************//**
		@class		SpriteDX12
		@brief		DX12で画像描画クラス
		@par		画像をローディングして画面に描画する
*//***************************************************************************/
class SpriteDX12
{
public:
	//! コンストラクタ
	SpriteDX12(UINT sprite_count, const char* filename = nullptr);

	//! デストラクタ
	~SpriteDX12();

	// 描画開始
	void Begin(ID3D12GraphicsCommandList* d3d_command_list);

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
		float r, float g, float b, float a);

	// 描画終了
	void End(ID3D12GraphicsCommandList* d3d_command_list);

	// テクスチャ幅取得
	int GetTextureWidth() const { return m_texture_width; }

	// テクスチャ高さ取得
	int GetTextureHeight() const { return m_texture_height; }

private:

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12Resource>			d3d_vb_resource;
		Microsoft::WRL::ComPtr<ID3D12Resource>			d3d_ib_resource;
		D3D12_VERTEX_BUFFER_VIEW						d3d_vbv;
		D3D12_INDEX_BUFFER_VIEW							d3d_ibv;
		Vertex* vertex_data = nullptr;
	};

private:

	std::vector<FrameResource>							m_frame_resources;
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