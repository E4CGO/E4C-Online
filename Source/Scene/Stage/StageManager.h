#pragma once

#include "TAKOEngine/Tool\Singleton.h"
#include "TAKOEngine/Physics\Collider.h"

#include "GameObject/ObjectManager.h"
#include "GameObject/GameObject.h"
#include "Scene/Stage/Stage.h"

class StageManager : public ObjectManager<GameObject>, public Singleton<StageManager>
{
	friend class Singleton<StageManager>;
protected:
	StageManager() = default;
	~StageManager() = default;

public:
	void SetStage(Stage* stage) { this->currentStage = stage; }
	Stage* GetStage() { return this->currentStage; }
public:
	Stage* currentStage;

	int stageNumber = 0;
};

#define STAGES StageManager::Instance()