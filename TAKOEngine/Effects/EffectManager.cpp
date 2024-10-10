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

void EffectManager::InitializeDX12()		// 初期化
{
	auto graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(T_GRAPHICS.GetDeviceDX12(), T_GRAPHICS.GetCommandQueue().d3d_command_queue.Get(), 3);

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	// Create a memory pool
	// メモリプールの作成
	::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Create a command list
	// コマンドリストの作成
	::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);

	// Sprcify rendering modules
	// 描画モジュールの設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

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

void EffectManager::RenderDX12(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビュー＆プロジェクション行列をEffekseerレンダラに設定
	efkRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	efkRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	Effekseer::Manager::UpdateParameter updateParameter;
	efkManager->Update(updateParameter);

	// Effekseer描画開始
	efkRenderer->BeginRendering();

	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();

	// Effekseer描画実行
	// マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
	// Draw()関数を実行する順序で制御できそう
	//efkManager->Draw();
	// Effekseer描画終了
	efkRenderer->EndRendering();
}