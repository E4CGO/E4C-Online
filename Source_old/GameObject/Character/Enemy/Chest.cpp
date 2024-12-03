#include "Chest.h"

#include <functional>

#include "TAKOEngine/Physics/Collider/SphereCollider.h"

#include "GameObject/Character/Enemy/ChestState.h"

#include "Scene/Stage/StageManager.h"

Chest::Chest(const char* filename) : Enemy(filename, 1.0f)
{
	enemyType = ENEMY_TYPE::CHEST_EMPTY;
	maxHp = hp = 1;
	atk = 0;
	moveSpeed = 0.0f;
	turnSpeed = 0.0f;

	showHp = false;

<<<<<<< HEAD:Source/GameObject/Character/Enemy/Chest.cpp
	// “–‚½‚è”»’è
	colliders[0] = new SphereCollider(Collider::COLLIDER_OBJ::ITEM, &transform);
=======
	// å½“ãŸã‚Šåˆ¤å®š
	colliders[0] = new SphereCollider(1.0f);
>>>>>>> 141e07936558e9fe621bfaedf68e3c4df6ed28a1:Source_old/GameObject/Character/Enemy/Chest.cpp

	stateMachine->RegisterState(CHEST_STATE::CLOSE, new ChestState::ChestCloseState(this));

	stateMachine->SetState(CHEST_STATE::CLOSE);
}

// è¡çªåˆ¤å®šæ›´æ–°
void Chest::UpdateColliders()
{
	//colliders[0]->SetPosition(GetNodePosition(DirectX::XMFLOAT3{ 0.0f, 0.5f, 0.0f }));
}

void Chest::OnDeath()
{
	stateMachine->ChangeState(CHEST_STATE::OPEN);
}

EmptyChest::EmptyChest() : Chest("Data/Model/Object/chest.glb")
{
	stateMachine->RegisterState(CHEST_STATE::OPEN, new ChestState::ChestOpenState(this, this->GetModel()->FindNode("chest_lid"), [chest = this](Enemy*) mutable {}));
}
EmptyChest::EmptyChest(std::function<void(Enemy*)> OnOpen) : Chest("Data/Model/Object/chest.glb")
{
	stateMachine->RegisterState(CHEST_STATE::OPEN, new ChestState::ChestOpenState(this, this->GetModel()->FindNode("chest_lid"), OnOpen));
}
void EmptyChest::SetOnOpen(std::function<void(Enemy*)> OnOpen)
{
	stateMachine->RegisterState(CHEST_STATE::OPEN, new ChestState::ChestOpenState(this, this->GetModel()->FindNode("chest_lid"), OnOpen));
}

VictoryChest::VictoryChest() : Chest("Data/Model/Object/chest_gold.glb")
{
	enemyType = ENEMY_TYPE::CHEST_COIN;
	stateMachine->RegisterState(CHEST_STATE::OPEN, new ChestState::ChestOpenState(this, this->GetModel()->FindNode("chest_gold_lid"), std::bind(&VictoryChest::OnOpen, this)));
}
void VictoryChest::OnOpen()
{
	STAGES.GetStage()->Finish();
}