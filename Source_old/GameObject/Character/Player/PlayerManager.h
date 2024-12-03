#pragma once

#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/Character/Player/Player.h"

class PlayerManager : public ObjectManager<Player>, public Singleton<PlayerManager>
{
	friend class Singleton<PlayerManager>;
protected:
	PlayerManager() = default;
	~PlayerManager() = default;
public:
	// デバッグ情報の表示
	void DrawDebugGUI();

	// デバッグプリミティブの表示
	void DrawDebugPrimitive();

	// 位置の初期化
	void InitializePosition();

	Player* GetPlayerById(int id);
};

#define PLAYERS PlayerManager::Instance()
