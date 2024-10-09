#pragma once

#include <unordered_map>

#include <DirectXMath.h>
#include <Effekseer/Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRendererDX11.h>
#include <EffekseerRendererDX12/EffekseerRendererDX12.h>

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/Effects/Effect.h"

class EffectManager : public Singleton<EffectManager>
{
	friend class Singleton<EffectManager>;
private:
	EffectManager() {};
	~EffectManager() {};

public:
	Effect* GetEffect(int index);

	void Initialize();				// ������
	void InitializeDX12();				// ������
	void Finalize();				// �I��
	void Update(float elapsedTime);	//�X�V
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection); // �`��
	void RenderDX12(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection); // �`��

	// Effeckseer�}�l�[�W���[�̎擾
	Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }
	Effekseer::ManagerRef GetEffekseerDX12Manager() { return efkManager; }
public:
	enum EFFECT_IDX {
		HIT_SLASH_EFFECT,
		HIT_SHOT_EFFECT,
		BOMB_EFFECT,
		FORCE_EFFECT,
	};
private:
	Effekseer::ManagerRef effekseerManager;
	EffekseerRenderer::RendererRef effekseerRenderer;

	Effekseer::ManagerRef efkManager;
	EffekseerRenderer::RendererRef efkRenderer;

	std::unordered_map<int, Effect*> effects;
};

#define EFFECTS EffectManager::Instance()