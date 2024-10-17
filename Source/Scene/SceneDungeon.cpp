#include "SceneDungeon.h"

#include <locale.h>
#include <profiler.h>
#include <algorithm>

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "TAKOEngine/Network/Network.h"
#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/GUI/Dialog.h"
#include "TAKOEngine/Physics/Collision.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Tool/Encode.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneDungeonState.h"
#include "Scene/SceneTitle.h"
#include "Scene/Stage/StageManager.h"
#include "Scene/Stage/TestingStage.h"

#include "GameObject/Character/Player/PlayerManager.h"
#include "GameObject/Projectile/ProjectileManager.h"

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "UI/Widget/WidgetText.h"
#include "UI/Widget/WidgetCrosshair.h"

#include "GameData.h"

using namespace SceneDungeonState;

SceneDungeon::SceneDungeon()
{

}

void SceneDungeon::Initialize()
{
	ID3D11Device* device = T_GRAPHICS.GetDevice();
	float screenWidth = T_GRAPHICS.GetScreenWidth();
	float screenHeight = T_GRAPHICS.GetScreenHeight();

	player = std::make_unique<Knight>();
	player->SetPosition({ 0, 10, 0 });
	player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));

	// Sprite Resource Preload
	for (auto& filename : spriteList)
	{
		spritePreLoad.insert(RESOURCE.LoadSpriteResource(filename));
	}
	// Model Resource Preload
	for (auto& filename : modelList)
	{
		modelPreLoad.insert(RESOURCE.LoadModelResource(filename));
	}

	//DeferredRendering
	deferredRendering->Initialize();

	//シャドウマップレンダラ
	shadowMapRenderer->Initialize();

	//ライト情報
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);
	shadowMapRenderer->SetShadowLight(dl);

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// 画角
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// 画面アスペクト比
		0.1f,														// ニアクリップ
		10000.0f);													// ファークリップ

	camera.SetLookAt(
		{ 0, 5.0f, 10.0f },	     // 視点
		{ 0, 0, 0 },	         // 注視点
		{ 0, 0.969f, -0.248f }); // 上ベクトル

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player.get());

	//ステージ
	//MapTile* wall = new MapTile("Data/Model/Dungeon/WallParent004.glb", 10.0f);
	MapTile* floor = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
	//MapTile* floor2 = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
	//floor2->SetPosition({ 4, 0, 0 });
	//stage = std::make_unique<MapTile>("Data/Model/Map/floor_dirt_large.gltf.glb", 1.0f);
	//stage = std::make_unique<TestingStage>();

	MAPTILES.Register(floor);

	//for (int i = 0; i <= 0; i++)
	//{
	//	MapTile* newTile = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
	//	newTile->SetPosition({ (4.0f * i), 0.0f, 0.0f });
	//	newTile->Update(0);
	//	MAPTILES.Register(newTile);
	//}

	//	モデルをレンダラーに登録
	iModel* list[] =
	{
		MAPTILES.get(0)->GetModel().get(),
	};
	for (iModel* model : list)
	{
		if (!model) continue;
		//shadowMapRenderer->ModelRegister(model);
	}

	stateMachine = std::make_unique<StateMachine<SceneDungeon>>();
	stateMachine->RegisterState(DungeonState::GENERATE, new SceneDungeonState::GenerateState(this));
	stateMachine->RegisterState(DungeonState::GAME, new SceneDungeonState::GameState(this));
	stateMachine->SetState(DungeonState::GENERATE);

	//stateMachine->Update(0);
}

void SceneDungeon::Finalize()
{
	modelPreLoad.clear();
	spritePreLoad.clear();
	shadowMapRenderer->Clear();
	CURSOR_ON;
	PLAYERS.Clear();
	MAPTILES.Clear();
	PROJECTILES.Clear();
	ENEMYCOLLISIONS.Clear();
	STAGES.Clear();
	UI.Clear();
	T_CONSOLE.Close();
}

void SceneDungeon::Update(float elapsedTime)
{
	stateMachine->Update(elapsedTime);
	player->Update(elapsedTime);

	// 雑に落ちないようにする
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	player->SetPosition({ playerPos.x, 0.0f, playerPos.z });

	MAPTILES.Update(elapsedTime);
	COLLISION.Update(elapsedTime);
	STAGES.Update(elapsedTime);

	// カメラ更新
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);
}

void SceneDungeon::Render()
{
	T_TEXT.Begin();
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	//シャドウマップ描画
	shadowMapRenderer->Render();
	rc.shadowMapData = shadowMapRenderer->GetShadowMapData();

	// 内容描画
	{
		//Deferred Rendering
		deferredRendering->SetDeferredRTV();

		player->Render(rc);

		//オブジェクト描画
		MAPTILES.Render(rc);					// マップ
		PLAYERS.Render(rc);						// プレイヤー
		PROJECTILES.Render(rc);					// 発射物
		STAGES.Render(rc);						// ステージオブジェクト
		EFFECTS.Render(camera.GetView(), camera.GetProjection()); 	// エフェクト
	}
#ifdef _DEBUG
	{
		PROJECTILES.DrawDebugPrimitive();
	}
#endif // _DEBUG

	// ラインレンダラ描画実行
	T_GRAPHICS.GetLineRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());

	//	ポストプロセス処理を行う
	{
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());
		postprocessingRenderer->Render(T_GRAPHICS.GetDeviceContext());
	}

	//DeferredRendering
	deferredRendering->Render();

	UI.Render(rc);						// インターフェース

	T_TEXT.End();

#ifdef _DEBUG
	ProfileDrawUI();
	shadowMapRenderer->DrawDebugGUI();
	deferredRendering->DrawDebugGUI();
#endif // _DEBUG
}