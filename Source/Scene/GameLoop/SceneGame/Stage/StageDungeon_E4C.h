//! @file StageDungeon_E4C.h
//! @note

#ifndef __INCLUDED_STAGE_DUNGEON_E4C_H__
#define __INCLUDED_STAGE_DUNGEON_E4C_H__

#include <memory>
#include <array>
#include <unordered_set>

#include "GameObject/ModelObject.h"
#include "GameObject/GameObject.h"
#include "GameObject/Character/Player/PlayerCharacter.h"

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"

#include "Map/DungeonData.h"
#include "Map/MapTile.h"
#include "Map/RoomBase.h"
#include "UI/Widget/WidgetText.h"
#include "Source/UI/Widget/WidgetPlayerHP.h"
#include "Source/UI/Widget/WidgetPauseMenu.h"

#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"
#include "TAKOEngine/Tool/GLTFImporter.h"

using namespace ns_RoomData;

class SceneGame_E4C;

class StageDungeon_E4C : public Stage
{
public:
	// コンストラクタ
	StageDungeon_E4C(SceneGame_E4C* scene) : m_pScene(scene), Stage() {}

	// 部屋の生成順番のセッター・ゲッター
	void SetRoomOrder(const std::vector<uint8_t>& newRoomOrder) { m_roomOrder = newRoomOrder; }
	std::vector<uint8_t> GetRoomOrder() { return m_roomOrder; }

	// 生成順番配列がない場合は自動生成、
	// ある場合は配列に従い生成を行う
	void GenerateDungeon();

	enum PHASE
	{
		NORMAL,
	};

	enum STATE
	{
	};

	void Initialize() override;

	void Finalize() override;

	void Update(float elapsedTime) override;

	void DefeatBoss() override;

	void Render() override;

	void RenderDX12() override;

	SceneGame_E4C* GetScene() { return m_pScene; }
private:
	// シーンGUI描画
	void DrawSceneGUI();

protected:
	SceneGame_E4C* m_pScene;

	std::unique_ptr<ThridPersonCameraController> cameraController;

	std::unique_ptr<RoomBase> rootRoom;
	std::vector<uint8_t> m_roomOrder;
	std::vector<AABB> m_roomAABBs;

	bool isLeader = true;

	std::unique_ptr <Plane> portal;

	std::unique_ptr<WidgetText> floorText;

	// Sprite Preload
	std::unordered_set<const char*> spriteList = {
		"",											// マスク
		// Setting UI
		"Data/Sprites/UI/start.png",
		"Data/Sprites/big_background.t.png"
	};

	std::unordered_set<std::shared_ptr<Sprite>> spritePreLoad;

	float transitionTime = 0.0f;
	float transitionDuration = 2.f;  // 5秒かけて移動
	int currentSegment = 0;
	int currentFloor = 0;

	// フレームバッファマネージャー
	FrameBufferManager* m_frameBuffer;

	// ポストエフェクト
	std::unique_ptr<PostprocessingRendererDX12>	postprocessingRenderer = std::make_unique<PostprocessingRendererDX12>();

	float m_sceneGlobalTimer = 0;
	float m_sceneTickTimer = 0;
};

#endif // !__INCLUDED_STAGE_DUNGEON_E4C_H__