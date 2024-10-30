#pragma once

#include "Scene/Scene.h"
#include "Scene/Stage/Stage.h"
#include "TAKOEngine/AI/StateMachine.h"

class SceneGame_E4C : public Scene
{
public:
	SceneGame_E4C() = default;
	~SceneGame_E4C() = default;

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;

	StateMachine<SceneGame_E4C>* GetStateMachine() { return stateMachine.get(); }

private:

	enum GAME_STATE
	{
		OPENWORLD,
		DUNGEON,
	};

	std::unique_ptr<Stage> stageOpenWorld;
	std::unique_ptr<Stage> stageDungeon;

	std::unique_ptr<StateMachine<SceneGame_E4C>> stateMachine;

	int stageNumber = 0;
};
