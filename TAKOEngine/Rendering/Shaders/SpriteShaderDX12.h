//! @file SpriteShaderDX12.h
//! @note 

#ifndef __GRAHICS_SPRITE_SHADER_DX12_H__
#define __GRAHICS_SPRITE_SHADER_DX12_H__

#include "TAKOEngine/Rendering/RenderContext.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"

//****************************************************
// @class SpriteShaderDX12
// @brief �X�v���C�g�V�F�[�_�[�̊��N���X(DX12)
// @par   
//****************************************************
class SpriteShaderDX12
{
protected:
	//�R���X�g���N�^
	SpriteShaderDX12() {};

public:
	//�f�X�g���N�^
	virtual ~SpriteShaderDX12() {};

	// �`��
	virtual void Render(const RenderContextDX12& rc, SpriteDX12* sprite) = 0;
};

//****************************************************
// @class DefaultSpriteShaderDX12
// @brief DX12�̕`��e�X�g
// @par   �e�N�X�`�������̂܂ܕ`�悷��
//****************************************************
class DefaultSpriteShaderDX12 : public SpriteShaderDX12
{
public:
	DefaultSpriteShaderDX12(ID3D12Device* device);
	~DefaultSpriteShaderDX12() override;

	void Render(const RenderContextDX12& rc, SpriteDX12* sprite) override;

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_d3d_pipeline_state;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_d3d_root_signature;
};

#endif // !__GRAHICS_SPRITE_SHADER_DX12_H__
