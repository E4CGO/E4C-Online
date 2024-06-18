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
	// �f�o�b�O���̕\��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�̕\��
	void DrawDebugPrimitive();

	// �ʒu�̏�����
	void InitializePosition();

	Player* GetPlayerById(int id);
};

#define PLAYERS PlayerManager::Instance()
