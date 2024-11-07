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

SceneDungeon::SceneDungeon()
{

}

void SceneDungeon::Initialize()
{
	ID3D11Device* device = T_GRAPHICS.GetDevice();
	float screenWidth = T_GRAPHICS.GetScreenWidth();
	float screenHeight = T_GRAPHICS.GetScreenHeight();

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

	PlayerCharacterData::CharacterInfo charaInfo = {
		true,
		"",
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};

	player = std::make_unique<PlayerCharacter>(charaInfo);
	player->SetPosition({ 0.0f, 0.0f, 0.0f });
	player->GetStateMachine()->ChangeState(static_cast<int>(PlayerCharacter::State::Idle));
	//player->SetKinematic(true);


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

	cameraController = std::make_unique<FreeCameraController>();
	cameraController->SyncCameraToController(camera);
	cameraController->SetEnable(true);

	//cameraControllerP = std::make_unique<ThridPersonCameraController>();
	//cameraControllerP->SyncCameraToController(camera);
	//cameraControllerP->SetEnable(true);
	//cameraControllerP->SetPlayer(player.get());



	// 部屋の生成を開始する
	roomTree = GenerateDungeon();

	// 出口を生成する
	int exitNum = (std::rand() % rootRoom->GetFarthestChild().size());
	RoomBase* exitRoom = rootRoom->GetFarthestChild().at(exitNum);
	exitRoom->PlaceExit();

	//MapTile* a = new MapTile("Data/Model/Stage/BigMap.glb");
	//MAPTILES.Register(a);

	MAPTILES.CreateSpatialIndex(5, 7);
}

void SceneDungeon::Finalize()
{
	modelPreLoad.clear();
	spritePreLoad.clear();
	shadowMapRenderer->Clear();

	delete rootRoom;

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
	// カメラ更新
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);
	//cameraControllerP->Update(elapsedTime);
	//cameraControllerP->SyncContrllerToCamera(camera);

	player->Update(elapsedTime);
	rootRoom->Update(elapsedTime);

	MAPTILES.Update(elapsedTime);
	//COLLISION.Update(elapsedTime);
	STAGES.Update(elapsedTime);

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
		//deferredRendering->SetDeferredRTV();

		player->Render(rc);
		//オブジェクト描画
		MAPTILES.Render(rc);					// マップ
		//PLAYERS.Render(rc);					// プレイヤー
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
	//deferredRendering->Render();

	UI.Render(rc);						// インターフェース

	T_TEXT.End();

#ifdef _DEBUG
	ProfileDrawUI();
	rootRoom->DrawDebugGUI();
	shadowMapRenderer->DrawDebugGUI();
	deferredRendering->DrawDebugGUI();
	player->DrawDebugGUI();
#endif // _DEBUG
}

void SceneDungeon::GenerateDungeon(std::vector<int> roomTree)
{
	int treeIndex = 0;
	rootRoom = new SimpleRoom1(nullptr, -1, roomTree, treeIndex);

	// 部屋のモデルを配置
	rootRoom->PlaceMapTile();
	for (RoomBase* room : rootRoom->GetAllChilds())
	{
		room->PlaceMapTile();
	}
}

std::vector<int> SceneDungeon::GenerateDungeon()
{
	std::vector<int> roomTree;
	std::vector<AABB> roomAABBs;

	rootRoom = new SimpleRoom1(nullptr, -1, roomAABBs);
	
	return roomTree;

	//// 部屋の当たり判定用配列
	//std::vector<AABB> roomAABBs;

	//rootRoom = new SimpleRoom1(nullptr, -1);
	//rootRoom->LoadMapTileData();
	//rootRoom->PlaceMapTile();
	//roomAABBs.emplace_back(rootRoom->GetAABB());

	//// 部屋のモデルを配置しつつ配列に保存
	//for (RoomBase* room : rootRoom->GetAll())
	//{
	//	room->LoadMapTileData();

	//	for (const AABB& aabb : roomAABBs)
	//	{
	//		IntersectionResult result;

	//		AABB aR = room->GetAABB();

	//		// 半径減らしとこ
	//		aR.radii = { 0.0f, 0.0f, 0.0f };
	//		//aR.radii = { 1.0f, 1.0f, 1.0f };

	//		if (Collision::IntersectAABBVsAABB(
	//			DirectX::XMLoadFloat3(&aR.position),
	//			DirectX::XMLoadFloat3(&aR.radii),
	//			DirectX::XMLoadFloat3(&aabb.position),
	//			DirectX::XMLoadFloat3(&aabb.radii),
	//			&result))
	//		{
	//			//int a = 0;
	//			roomTree.emplace_back(DungeonData::END_ROOM);
	//		}
	//		else
	//		{
	//			room->PlaceMapTile();
	//			roomAABBs.emplace_back(room->GetAABB());
	//			roomTree.emplace_back(room->GetRoomType());
	//		}
	//	}
	//	//room->PlaceMapTile();
	//	//roomAABBs.emplace_back(room->GetAABB());
	//	//roomTree.emplace_back(room->GetRoomType());
	//	//roomTree.emplace_back(room->GetRoomType());
	//}
	//return roomTree;
}