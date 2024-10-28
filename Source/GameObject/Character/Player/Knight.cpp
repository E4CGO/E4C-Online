#include "Knight.h"

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Rendering/LineRenderer.h"

#include "GameObject/Character/Player/KnightState.h"

Knight::Knight() : Player("Data/Model/Character/Knight.glb", 1.0f)
{
	hp = maxHp = 180;
	type = PLAYER_CLASS::KNIGHT;

	model->FindNode("1H_Sword_Offhand")->visible = false;
	model->FindNode("2H_Sword")->visible = false;
	model->FindNode("Badge_Shield")->visible = false;
	model->FindNode("Round_Shield")->visible = false;
	model->FindNode("Spike_Shield")->visible = false;

	// 攻撃判定
	attackColliders[AttackCollider::Sword_1] = new SphereCollider(0.3f);
	attackColliders[AttackCollider::Sword_2] = new SphereCollider(0.3f);
	attackColliders[AttackCollider::Sword_3] = new SphereCollider(0.3f);
	attackColliders[AttackCollider::Shield] = new SphereCollider(0.8f);
	EnableAttackColliders(false);

	stateMachine->RegisterState(static_cast<int>(Player::State::AttackNormal), new KnightState::AttackNormalState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_1, new KnightState::AttackNormalState_1(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_2, new KnightState::AttackNormalState_2(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackNormal), ATTACK_STATE::ATTACK_3, new KnightState::AttackNormalState_3(this));

	stateMachine->RegisterState(static_cast<int>(Player::State::AttackSpecial), new KnightState::AttackSpecialState(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_START, new KnightState::AttackSpecialStateStart(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCKING, new KnightState::AttackSpecialStateBlocking(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_ATTACK, new KnightState::AttackSpecialStateAttack(this));
	stateMachine->RegisterSubState(static_cast<int>(Player::State::AttackSpecial), BLOCK_STATE::BLOCK_HIT, new KnightState::AttackSpecialStateHit(this));

	stateMachine->RegisterState(static_cast<int>(Player::State::Skill_1), new KnightState::Skill1State(this));
	mpCost[static_cast<int>(Player::State::Skill_1)] = 20.0f;
	skillTimer[static_cast<int>(Player::State::Skill_1)] = 8.0f;
}

void Knight::AttackCollision()
{
	//Sword
	switch (stateMachine->GetStateIndex())
	{
	case static_cast<int>(State::AttackNormal): // 一般攻撃
		for (int i = AttackCollider::Sword_1; i <= AttackCollider::Sword_3; i++)
		{
			CollisionVsEnemies(attackColliders[i], atk, true, 1.0f, EffectManager::EFFECT_IDX::HIT_SLASH_EFFECT, 0.4f);
		}
		break;
	case static_cast<int>(State::AttackSpecial): // 防御
		if (stateMachine->GetState()->GetSubStateIndex() == BLOCK_STATE::BLOCK_ATTACK) CollisionVsEnemies(attackColliders[AttackCollider::Shield], atk / 5, true, 4.0f);
		break;
	case static_cast<int>(State::Skill_1): // 防御
		CollisionVsEnemies(attackColliders[AttackCollider::Shield], atk, true, 15.0f, EffectManager::EFFECT_IDX::HIT_SHOT_EFFECT, 0.5f);
		break;
	}
}
void Knight::RenderTrail()
{
	switch (stateMachine->GetStateIndex())
	{
	case static_cast<int>(State::AttackNormal): // 一般攻撃
		for (int i = AttackCollider::Sword_1; i <= AttackCollider::Sword_3; i++)
		{
			const iModel::Node* swordnode1 = this->GetSwordtrailNode();
			LineRenderer* sword = Graphics::Instance().GetLineRenderer();
			{
				{
					for (int i = 0; i < 2; ++i)
					{
						for (int j = MAX_POLYGON - 1; j > 0; --j)
						{
							trailPosition[i][j] = trailPosition[i][j - 1];
						}
					}
				}
				{
					// 剣の原点から根本と先端までのオフセット値
					DirectX::XMVECTOR RootOffset = DirectX::XMVectorSet(120.f, 0, 0, 0);
					DirectX::XMVECTOR TipOffset = DirectX::XMVectorSet(-120.f, 0, 0, 0);

					DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&swordnode1->worldTransform);
					DirectX::XMVECTOR Root = DirectX::XMVector3Transform(RootOffset, W);
					DirectX::XMVECTOR Tip = DirectX::XMVector3Transform(TipOffset, W);
					DirectX::XMStoreFloat3(&trailPosition[0][0], Root);
					DirectX::XMStoreFloat3(&trailPosition[1][0], Tip);
				}

				{

					const int division = 10;
					for (int i = 0; i < MAX_POLYGON - 3; ++i)
					{
						DirectX::XMVECTOR Root0 = DirectX::XMLoadFloat3(&trailPosition[0][i + 0]);
						DirectX::XMVECTOR Root1 = DirectX::XMLoadFloat3(&trailPosition[0][i + 1]);
						DirectX::XMVECTOR Root2 = DirectX::XMLoadFloat3(&trailPosition[0][i + 2]);
						DirectX::XMVECTOR Root3 = DirectX::XMLoadFloat3(&trailPosition[0][i + 3]);

						DirectX::XMVECTOR Tips0 = DirectX::XMLoadFloat3(&trailPosition[1][i + 0]);
						DirectX::XMVECTOR Tips1 = DirectX::XMLoadFloat3(&trailPosition[1][i + 1]);
						DirectX::XMVECTOR Tips2 = DirectX::XMLoadFloat3(&trailPosition[1][i + 2]);
						DirectX::XMVECTOR Tips3 = DirectX::XMLoadFloat3(&trailPosition[1][i + 3]);

						for (int j = 0; j < division; ++j)
						{
							float t = j / static_cast<float>(division - 1); // division - 1で割ることで、tの値が0から1になるように調整

							DirectX::XMVECTOR Root = DirectX::XMVectorCatmullRom(Root0, Root1, Root2, Root3, t);
							DirectX::XMVECTOR Tip = DirectX::XMVectorCatmullRom(Tips0, Tips1, Tips2, Tips3, t);

							DirectX::XMFLOAT3 rootFloat3;
							DirectX::XMFLOAT3 tipFloat3;
							DirectX::XMStoreFloat3(&rootFloat3, Root);
							DirectX::XMStoreFloat3(&tipFloat3, Tip);

							
							sword->AddVertex(rootFloat3, {1,1,1,1});
							sword->AddVertex(tipFloat3, {1,1,1,1});
							

						}
					}
				}
			}
		}
		break;
	}
}
void Knight::OnDamage(const HitResult& hit, int damage)
{
	if (hurtCoolTime > 0.0f) return;
	hurtCoolTime = 0.5f;
	TPSCamera.Shake(0.2f, 0.5f);

	float vx = hit.position.x - position.x;
	float vz = hit.position.z - position.z;
	Turn(1.0f, vx, vz, DirectX::XMConvertToRadians(360));

	// ブローク
	if (
		stateMachine->GetStateIndex() == static_cast<int>(State::AttackSpecial) &&
		AngleBetweenXMFLOAT3(GetFront(), DirectX::XMFLOAT3{ vx, 0, vz }) <= DirectX::XMConvertToRadians(90)
		)
	{
		stateMachine->ChangeSubState(BLOCK_STATE::BLOCK_HIT);
	}
	else
	{
		hp -= damage;
		if (hp < 0)
		{
			hp = 0;
			stateMachine->ChangeState(static_cast<int>(State::Death));
		}
		else
		{
			stateMachine->ChangeState(static_cast<int>(State::Hurt));
		}
	}
}

void Knight::UpdateColliders()
{
	// 当たり判定
	Player::UpdateColliders();

	// 攻撃判定
	float Yoffset = 0.2f;
	attackColliders[AttackCollider::Sword_1]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));
	Yoffset += attackColliders[AttackCollider::Sword_1]->GetScale().x + attackColliders[AttackCollider::Sword_2]->GetScale().x;
	attackColliders[AttackCollider::Sword_2]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));
	Yoffset += attackColliders[AttackCollider::Sword_2]->GetScale().x + attackColliders[AttackCollider::Sword_3]->GetScale().x;
	attackColliders[AttackCollider::Sword_3]->SetPosition(GetNodePosition("1H_Sword", DirectX::XMFLOAT3{ 0.0f, Yoffset, 0.0f }));

	attackColliders[AttackCollider::Shield]->SetPosition(GetNodePosition("Rectangle_Shield", DirectX::XMFLOAT3{ 0.0f, 0.0f, -0.1f }));
}

//剣の軌跡ノード取得
const iModel::Node* Knight::GetSwordtrailNode()
{
	iModel::Node* node = model->FindNode("2H_Sword");

	return node;

}