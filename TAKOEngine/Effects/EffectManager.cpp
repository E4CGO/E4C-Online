#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "EffectManager.h"

void EffectManager::Initialize()		// 初期化
{
	// Effekseerレンダラ生成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
		T_GRAPHICS.GetDevice(),
		T_GRAPHICS.GetDeviceContext(),
		2048
	);

	// Effekseerマネージャー生成
	effekseerManager = Effekseer::Manager::Create(2048);

	// Effekseerレンダラの各種設定（特別なカスタマイズをしない場合は定型的に以下の設定でOK）
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でOK）
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	// Effekseerを左手座標系で計算する
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	effects[EFFECT_IDX::HIT_SLASH_EFFECT] = new Effect("Data/Effects/Hit01.efk");
	effects[EFFECT_IDX::HIT_SHOT_EFFECT] = new Effect("Data/Effects/Hit02.efk");
	effects[EFFECT_IDX::BOMB_EFFECT] = new Effect("Data/Effects/Bomb.efk");
}

Effect* EffectManager::GetEffect(int index)
{
	if (effects.find(index) == effects.end()) return nullptr;
	return effects[index];
}

void EffectManager::Finalize()
{
	for (std::pair<int, Effect*> effect : effects)
	{
		delete effect.second;
	}
	effects.clear();
}

void EffectManager::Update(float elapsedTime)
{
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビュー＆プロジェクション行列をEffekseerレンダラに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// Effekseer描画開始
	effekseerRenderer->BeginRendering();
	// Effekseer描画実行
	// マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
	// Draw()関数を実行する順序で制御できそう
	effekseerManager->Draw();
	// Effekseer描画終了
	effekseerRenderer->EndRendering();
}