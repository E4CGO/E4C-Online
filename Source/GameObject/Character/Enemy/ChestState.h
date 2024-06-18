#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include "GameObject/Character/Enemy/Chest.h"

namespace ChestState
{
	// 蓋閉め
	class ChestCloseState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		ChestCloseState(Enemy* enemy) : HierarchicalState<Enemy>(enemy) {};
		// デストラクタ
		~ChestCloseState() {};
		// ステートに入った時のメソッド
		void Enter() override {};
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override {};
		// ステートから出ていくときのメソッド
		void Exit() override {};
	};
	// 蓋開く
	class ChestOpenState : public HierarchicalState<Enemy>
	{
	public:
		// コンストラクタ
		ChestOpenState(Enemy* enemy, Model::Node* coverNode, std::function<void(Enemy*)> OnOpen = nullptr) : HierarchicalState<Enemy>(enemy), coverNode(coverNode), OnOpen(OnOpen) {};
		// デストラクタ
		~ChestOpenState() {};
		// ステートに入った時のメソッド
		void Enter() override;
		// ステートで実行するメソッド
		void Execute(float elapsedTime) override;
		// ステートから出ていくときのメソッド
		void Exit() override {};
	protected:
		Model::Node* coverNode; // 蓋ノード
		std::function<void(Enemy*)> OnOpen = nullptr;
		float timer;
		float openAngle = DirectX::XMConvertToRadians(-120.0f);
	};
}
