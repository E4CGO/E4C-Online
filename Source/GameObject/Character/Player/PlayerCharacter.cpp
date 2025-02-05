	//! @file PlayerCharacter.cpp
//! @note

#include "PlayerCharacter.h"

#include <profiler.h>
#include <iostream>

#include "Map/MapTileManager.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Physics/CollisionManager.h"
#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/CapsuleCollider.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player/State/PlayerCharacterState.h"

#include "GameObject/Character/Enemy/EnemyManager.h"

#include "GameData.h"

#include "TAKOEngine/Tool/Mathf.h"
#include "TAKOEngine/Rendering/LineRenderer.h"

PlayerCharacter::PlayerCharacter(uint32_t id, const char* name, const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]) : Character()
{
	scale = { 0.5f, 0.5f, 0.5f };
	radius = 0.4f;
	moveSpeed = 10.0f;
	turnSpeed = DirectX::XMConvertToRadians(1440);
	jumpSpeed = 20.0f;
	dodgeSpeed = 20.0f;

	stateMachine = new StateMachine<PlayerCharacter>;
	RegisterCommonState();
	stateMachine->SetState(static_cast<int>(STATE::WAITING));

	mpCost[static_cast<int>(STATE::DODGE)] = 0.0f;

	// 衝突判定
	SetMoveCollider({ { 0, radius / scale.y, 0 }, radius }, Collider::COLLIDER_OBJ::PLAYER);

	m_pColliders.clear();
	// ヒット判定
	Capsule capsule{};
	capsule.radius = radius;
	capsule.position = { 0, capsule.radius / scale.y, 0 };
	capsule.direction = { 0.0f, 1.0f, 0.0f };
	capsule.length = height - capsule.radius * 2;
	SetCollider(COLLIDER_ID::COL_BODY, capsule, Collider::COLLIDER_OBJ::PLAYER, &transform);

	m_client_id = id;
	this->m_name = name;

	LoadAppearance(appearance);

	// DebugPrimitive用
	m_sphere = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());

	m_EffectZone = std::make_unique<ZoneObject>();
	m_EffectCharge = std::make_unique<ChargeObject>();
	m_EffectHealing = std::make_unique<HealingObject>();
}

PlayerCharacter::PlayerCharacter(const PlayerCharacterData::CharacterInfo& dataInfo) : Character()
{
	scale = { 0.5f, 0.5f, 0.5f };
	moveSpeed = 10.0f;
	turnSpeed = DirectX::XMConvertToRadians(720);
	jumpSpeed = 20.0f;
	dodgeSpeed = 20.0f;

	stateMachine = new StateMachine<PlayerCharacter>;
	RegisterCommonState();
	stateMachine->SetState(static_cast<int>(STATE::WAITING));

	mpCost[static_cast<int>(STATE::DODGE)] = 0.0f;

	// 衝突判定
	SetMoveCollider({ { 0, radius, 0 }, radius }, Collider::COLLIDER_OBJ::PLAYER);
	m_pColliders.clear();

	this->m_name = dataInfo.name;
	LoadAppearance(dataInfo.pattern);

	// DebugPrimitive用
	m_sphere = std::make_unique<SphereRenderer>(T_GRAPHICS.GetDeviceDX12());

	// 効果
	{
		m_EffectZone = std::make_unique<ZoneObject>();
		m_EffectCharge = std::make_unique<ChargeObject>();
		m_EffectHealing = std::make_unique<HealingObject>();
	}
}

/**************************************************************************//**
	@brief		外見パターンを読み取る
	@param[in]	appearance
	@return		なし
*//***************************************************************************/
void PlayerCharacter::LoadAppearance(const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM])
{
	ModelObject::CleanModels();

	for (uint8_t i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
	{
		PlayerCharacterData::Instance().LoadAppearance(this, i, appearance[i]);
	}

	stateMachine->SetState(static_cast<int>(STATE::WAITING));
}

PlayerCharacter::~PlayerCharacter()
{
	delete stateMachine;
}

void PlayerCharacter::RegisterCommonState()
{
	//stateMachine->RegisterState(static_cast<int>(PlayerCharacter::State::Idle), new PlayerCharacterState::IdleState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::MOVE), new PlayerCharacterState::MoveState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::JUMP), new PlayerCharacterState::JumpState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::FALL), new PlayerCharacterState::FallState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::LAND), new PlayerCharacterState::LandState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::DODGE), new PlayerCharacterState::DodgeState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::HURT), new PlayerCharacterState::HurtState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::DEATH), new PlayerCharacterState::DeathState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::WAITING), new PlayerCharacterState::WaitState(this));
	stateMachine->RegisterState(static_cast<int>(PlayerCharacter::STATE::READY), new PlayerCharacterState::ReadyState(this));
}

void PlayerCharacter::UpdateTarget()
{
	// レイの開始位置は足元より少し上
	DirectX::XMFLOAT3 start = CameraManager::Instance().GetCamera()->GetEye();
	// レイの終点位置は移動後の位置
	DirectX::XMFLOAT3 end = CameraManager::Instance().GetCamera()->GetFront() * 100.0f + start;
	// レイキャストによる地面判定
	HitResult hit;
	if (ENEMIES.RayCast(start, end, hit))
	{
		target = hit.position;
	}
	else
	{
		target = end;
	}
}

void PlayerCharacter::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		{
			ProfileScopedSection_2("UpdateHorizontalMove", ImGuiControl::Profiler::Purple);
			// 水平移動地
			float mx = velocity.x * elapsedTime;
			float mz = velocity.z * elapsedTime;

			// マップ
			if (m_pMoveCollider != nullptr) {
				if (IsPlayer())
				{
					// 重いときにスフィアキャストを分割して行う
					int solver = 1;
					float t = elapsedTime;
					while (t > 0.02f)
					{
						t -= 0.02f;
						solver++;
					}
					mx /= solver;
					mz /= solver;
					XMFLOAT3 pos = position + XMFLOAT3{ 0, height * 0.5f, 0 };
					XMFLOAT3 dir = { mx, 0.0f, mz };
					float dist = XMFLOAT3Length(dir);
					dir = XMFLOAT3Normalize(dir);

					Capsule sphereCast;
					sphereCast.position = pos;
					sphereCast.direction = dir;
					sphereCast.length = dist;
					sphereCast.radius = radius;

					for (int i = 0; i < solver; i++)
					{
						if (MAPTILES.IntersectCapsuleVsMap(sphereCast))
						{
							pos = sphereCast.position + sphereCast.direction * sphereCast.length;
							sphereCast.position = pos;
							//position = pos - XMFLOAT3{ 0, height * 0.5f, 0 };
						}
						else
						{
							pos.x += mx;
							pos.z += mz;
							sphereCast.position = pos;
							//position.x += mx;
							//position.z += mz;
						}
					}
					position = pos - XMFLOAT3{ 0, height * 0.5f, 0 };
					m_pMoveCollider->SetPosition(position + XMFLOAT3{ 0, radius, 0 });
				}
				else
				{
					position.x += mx;
					position.z += mz;
				}
			}
			else
			{
				position.x += mx;
				position.z += mz;
			}
		}
	}
}

void PlayerCharacter::PositionAdjustment()
{
	if (IsPlayer())
	{
		if (m_pMoveCollider)
		{
			if (CollisionVsEnemies() || (XMFLOAT3LengthSq(velocity) > 0.0f))
			{
				if (m_pMoveCollider->CollisionVsMap())
				{
					position = m_pMoveCollider->GetPosition();
					position.y -= radius;
				}
			}
		}
	}
}

void PlayerCharacter::UpdateColliders()
{
	if (IsPlayer())
	{
		if (m_pMoveCollider)
		{
			m_pMoveCollider->Update();
			for (const std::pair<uint8_t, Collider*>& collider : m_pColliders)
			{
				collider.second->Update();
			}
		}
	}
}

bool  PlayerCharacter::CollisionVsEnemies()
{
	bool isHit = false;

	if (!m_pMoveCollider->IsEnable()) return isHit;

	HitResult hit;
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		if (!enemy->GetMoveCollider()) continue;

		if (m_pMoveCollider->Collision(enemy->GetMoveCollider(), {}, hit))
		{
			hit.position.y = position.y;
			hit.normal.y = 0.0f;
			position = hit.position + hit.normal * radius;
			m_pMoveCollider->SetPosition(position);

			//position.y -= radius;
			isHit = true;
		}

		for (auto& col : enemy->GetColliders())
		{
			if (col.second->GetOBJType() & Collider::COLLIDER_OBJ::ENEMY)
			{
				if (m_pMoveCollider->Collision(col.second, {}, hit))
				{
					hit.position.y = position.y;
					hit.normal.y = 0.0f;
					position = hit.position + hit.normal * radius;
					m_pMoveCollider->SetPosition(position);

					//position.y -= radius;
					isHit = true;
				}
			}
		}
	}
	return isHit;
}

//bool  PlayerCharacter::CollisionVsEnemyAttack(Collider* collider, int damage, bool power, float force, int effectIdx, float effectScale)
//{
//	bool isHit = false;
//	for (Enemy*& enemy : ENEMIES.GetAll())
//	{
//		if (enemy->GetHurtCoolTime() > 0.0f) continue; // 無敵時間
//		for (std::pair<int, Collider*>enemyCollider : enemy->GetColliders())
//		{
//			HitResult hit;
//			if (collider->Collision(enemyCollider.second, {}, hit))
//			{
//				ENEMYCOLLISIONS.Register(enemy, enemyCollider.first, damage, power, hit.normal * -force);
//				if (effectIdx > -1)
//				{
//					EFFECTS.GetEffect(effectIdx)->Play(hit.position, effectScale);
//				}
//
//				enemy->SetHurtCoolTime(0.4f);
//				isHit = true;
//				std::cout << "Hit enemy_id: " << enemy->GetEnemyId() << std::endl;
//				break;
//			}
//		}
//	}
//	return isHit;
//}

//void PlayerCharacter::AttackEnemy(Collider* attackCol, Collider* enemyCol)
//{
//	for (GameObject* enemy : attackCol->GetHitOthers())
//	{
//		// 既にヒットした敵には当たらない
//		if (enemy == enemyCol->GetOwner()) return;
//	}
//
//	Enemy* enemy = static_cast<Enemy*>(enemyCol->GetOwner());
//	attackCol->RegisterHitOthers(enemy);
//	ATTACK_DATA attack(attackCol->GetHitDamage(), {}, false);
//	enemy->OnDamage(attack);
//}

void PlayerCharacter::UpdateInput()
{
	float ax = 0.0f;
	float ay = 0.0f;
	inputDirection = {}; // ゼロ
	if (GAME_SETTING.KeyboardInput)
	{
		// キーボード
		if (T_INPUT.KeyPress('W')) input |= Input_Up;
		if (T_INPUT.KeyPress('S')) input |= Input_Down;
		if ((input & (Input_Up | Input_Down)) == (Input_Up | Input_Down))
		{
			input &= ~(Input_Up | Input_Down);
		}
		if (T_INPUT.KeyPress('A')) input |= Input_Left;
		if (T_INPUT.KeyPress('D')) input |= Input_Right;
		if ((input & (Input_Left | Input_Right)) == (Input_Left | Input_Right))
		{
			input &= ~(Input_Left | Input_Right);
		}

		if (T_INPUT.KeyDown(VK_SPACE)) input |= Input_Jump;
		if (T_INPUT.KeyDown(VK_CONTROL) && mp >= GetMpCost(static_cast<int>(STATE::DODGE))) input |= Input_Dodge;

		if (T_INPUT.KeyPress(VK_LBUTTON) && !TentacleLib::isShowCursor()) input |= Input_Attack_N;
		if (T_INPUT.KeyPress(VK_RBUTTON) && !TentacleLib::isShowCursor()) input |= Input_Attack_S;

		if (
			T_INPUT.KeyPress('1')
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_1))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_1)) == 0.0f
			) input |= Input_Skill_1;

		if (
			T_INPUT.KeyPress('2')
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_2))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_2)) == 0.0f
			) input |= Input_Skill_2;

		if (
			T_INPUT.KeyPress('3')
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_3))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_3)) == 0.0f
			) input |= Input_Skill_3;

		if (
			T_INPUT.KeyPress('4')
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_4))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_4)) == 0.0f
			) input |= Input_Skill_4;

		if (T_INPUT.KeyUp(VK_LBUTTON)) input |= Input_R_Attack_N;
		if (T_INPUT.KeyUp(VK_RBUTTON)) input |= Input_R_Attack_S;
		if (T_INPUT.KeyUp('1')) input |= Input_R_Skill_1;
		if (T_INPUT.KeyUp('2')) input |= Input_R_Skill_2;
		if (T_INPUT.KeyUp('3')) input |= Input_R_Skill_3;
		if (T_INPUT.KeyUp('4')) input |= Input_R_Skill_4;

		// 入力による移動量計算
		if (input & Input_Left) ax -= 1.0f;
		if (input & Input_Right) ax += 1.0f;
		if (input & Input_Up) ay += 1.0f;
		if (input & Input_Down) ay -= 1.0f;
	}
	else
	{
		// ゲームパッド
		if (T_INPUT.GamePadKeyDown(GAME_PAD_BTN::A)) input |= Input_Jump;
		if (T_INPUT.GamePadKeyDown(GAME_PAD_BTN::B) && mp >= GetMpCost(static_cast<int>(STATE::DODGE))) input |= Input_Dodge;

		if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::X) && !TentacleLib::isShowCursor()) input |= Input_Attack_N;
		if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::LTRIGGER) && !TentacleLib::isShowCursor()) input |= Input_Attack_S;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::LSHOULDER)
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_1))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_1)) == 0.0f
			) input |= Input_Skill_1;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::RSHOULDER)
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_2))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_2)) == 0.0f
			) input |= Input_Skill_2;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::RTRIGGER)
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_3))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_3)) == 0.0f
			) input |= Input_Skill_3;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::Y)
			&& mp >= GetMpCost(static_cast<int>(STATE::SKILL_4))
			&& GetSkillTimerTime(static_cast<int>(STATE::SKILL_4)) == 0.0f
			) input |= Input_Skill_4;

		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::X)) input |= Input_R_Attack_N;
		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::LTRIGGER)) input |= Input_R_Attack_S;
		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::LSHOULDER)) input |= Input_R_Skill_1;
		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::RSHOULDER)) input |= Input_R_Skill_2;
		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::RTRIGGER)) input |= Input_R_Skill_3;
		if (T_INPUT.GamePadKeyUp(GAME_PAD_BTN::Y)) input |= Input_R_Skill_4;
		//移動量計算
		ax = T_INPUT.GetGamePadLAxis().x;
		ay = T_INPUT.GetGamePadLAxis().y;
	}

	// カメラ方向とスティックの入力値によって進行方向を計算する
	const DirectX::XMFLOAT3& cameraRight = CameraManager::Instance().GetCamera()->GetRight();
	const DirectX::XMFLOAT3& cameraFront = CameraManager::Instance().GetCamera()->GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする
	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f) {
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}
	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f) {
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	inputDirection.x = cameraRightX * ax + cameraFrontX * ay;
	inputDirection.y = cameraRightZ * ax + cameraFrontZ * ay;
}

/*
* 入力方向取得
* 入力なし：キャラの向き
*/
DirectX::XMFLOAT2 PlayerCharacter::GetInputDirection()
{
	DirectX::XMFLOAT2 direction = {};
	if (inputDirection.x == 0 && inputDirection.y == 0) // 方向入力なし
	{
		// キャラ向き依存
		direction.x = sinf(angle.y);
		direction.y = cosf(angle.y);
	}
	else
	{
		// 入力依存
		direction.x = inputDirection.x;
		direction.y = inputDirection.y;
	}
	return direction;
}

// スキルクールタイム管理
float PlayerCharacter::GetSkillTimerTime(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer;
}
float PlayerCharacter::GetSkillTimerRate(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer / skillTimer[idx].time;
}
void PlayerCharacter::ResetSkillTimer(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return;
	skillTimer[idx].currentTimer = skillTimer[idx].time;
}
void PlayerCharacter::UpdateSkillTimers(float elapsedTime)
{
	for (std::pair<int, SkillTimer> timer : skillTimer)
	{
		skillTimer[timer.first].currentTimer -= elapsedTime;
		if (skillTimer[timer.first].currentTimer <= 0.0f) skillTimer[timer.first].currentTimer = 0.0f;
	}
}
void PlayerCharacter::SwordTrail()
{
	const iModel::Node* swordnode1 = GetSwordTrailNode();
	LineRenderer* sword = Graphics::Instance().GetLineRenderer();
	LineRendererDX12* dx12_sword = Graphics::Instance().GetLineRendererDX12();

	// トレイルデータをシフト
	for (int i = 0; i < 2; ++i)
	{
		for (int j = MAX_POLYGON - 1; j > 0; --j)
		{
			trailPosition[i][j] = trailPosition[i][j - 1];
		}
	}
	// 剣の原点から根本と先端までのオフセット値
	DirectX::XMVECTOR RootOffset = DirectX::XMVectorSet(0, 150.f, 0, 0);
	DirectX::XMVECTOR TipOffset = DirectX::XMVectorSet(0, -30.f, 0, 0);
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&swordnode1->worldTransform);
	DirectX::XMVECTOR Root = DirectX::XMVector3Transform(RootOffset, W);
	DirectX::XMVECTOR Tip = DirectX::XMVector3Transform(TipOffset, W);
	DirectX::XMStoreFloat3(&trailPosition[0][0], Root);
	DirectX::XMStoreFloat3(&trailPosition[1][0], Tip);
	if (IsTrail())
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
				float texcoordV = i / static_cast<float>(MAX_POLYGON - 3); // 軌跡全体に沿ったV座標を計算
				DirectX::XMVECTOR Root = DirectX::XMVectorCatmullRom(Root0, Root1, Root2, Root3, t);
				DirectX::XMVECTOR Tip = DirectX::XMVectorCatmullRom(Tips0, Tips1, Tips2, Tips3, t);

				DirectX::XMFLOAT3 rootFloat3, tipFloat3;
				DirectX::XMStoreFloat3(&rootFloat3, Root);
				DirectX::XMStoreFloat3(&tipFloat3, Tip);

				if (T_GRAPHICS.isDX11Active)
				{
					sword->AddVertex(rootFloat3, { 1, 1, 1, 1 }, { 0, texcoordV });
					sword->AddVertex(tipFloat3, { 1, 1, 1, 1 }, { 1, texcoordV });
				}
				else
				{
					dx12_sword->AddVertex(rootFloat3, { 1, 1, 1, 1 }, { 0, texcoordV });
					dx12_sword->AddVertex(tipFloat3, { 1, 1, 1, 1 }, { 1, texcoordV });
				}
			}
		}
	}
}
//剣の軌跡ノード取得
const iModel::Node* PlayerCharacter::GetSwordTrailNode()
{
	iModel::Node* node = this->GetModel()->FindNode("JOT_C_Blade");

	return node;
}
void PlayerCharacter::Update(float elapsedTime)
{
	SwordTrail();

	if (m_hitStop)
	{
		m_stopTimer += elapsedTime;
		if (m_stopTimer > m_stopTime)
		{
			m_hitStop = false;
			m_stopTimer = 0;
		}
		return;
	}

	std::lock_guard<std::mutex> lock(m_mut);
	{
		ProfileScopedSection_2("input", ImGuiControl::Profiler::Red);
		if (IsPlayer()) // 自機限定 ステート管理
		{
			input = 0;
			UpdateTarget();
			UpdateInput();
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
			if (m_tempData.time > 0.0f)
			{
				m_tempData.timer += elapsedTime;
				float rate = m_tempData.timer / m_tempData.time;
				position = {
					Mathf::Lerp(m_tempData.position.x, m_tempData.sync_data.position[0], rate),
					//Mathf::Lerp(m_tempData.position.y, m_tempData.sync_data.position[1], rate),
					position.y,
					Mathf::Lerp(m_tempData.position.z, m_tempData.sync_data.position[2], rate),
				};
				angle.y = Mathf::LerpRadian(m_tempData.angle, m_tempData.sync_data.rotate, rate);
			}
			if (m_tempData.position.y - position.y > 10.0f)
			{
				position.y = m_tempData.position.y;
			}
		}
	}
	{
		ProfileScopedSection_2("stateMachine", ImGuiControl::Profiler::Blue);
		stateMachine->Update(elapsedTime);
	}
	{
		ProfileScopedSection_2("SkillTimers", ImGuiControl::Profiler::Green);
		UpdateSkillTimers(elapsedTime);
	}
	{
		//ProfileScopedSection_2("character", ImGuiControl::Profiler::Purple);

		Character::Update(elapsedTime);

		// 効果
		{
			m_EffectZone->SetPosition(position);
			m_EffectZone->Update(elapsedTime);
			m_EffectCharge->SetPosition(position);
			m_EffectCharge->Update(elapsedTime);
			m_EffectHealing->Update(elapsedTime);

			if (m_DefenceBuffTimeoutTimer > 0)
			{
				m_DefenceBuffTimeoutTimer -= elapsedTime;
			}
			else
			{
				m_isDefenceBuff = false;
			}
		}
	}
	iModel::Node* node = this->GetModel()->FindNode("Mesh_0");
}

void PlayerCharacter::Render(const RenderContext& rc)
{
	Character::Render(rc);

	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();
	DirectX::XMFLOAT3 namePos = this->position + DirectX::XMFLOAT3{ 0, 2.2f, 0 };
	float dot = XMFLOAT3Dot(front, namePos - eye);

	LineRenderer* sword = Graphics::Instance().GetLineRenderer();
	sword->Render(T_GRAPHICS.GetDeviceContext(), rc.camera->GetView(), rc.camera->GetProjection());
	if (dot < 0.0f) return;

#ifdef _DEBUG
	m_pColliders[COLLIDER_ID::COL_BODY]->DrawDebugPrimitive({ 0, 1, 0, 1 });
	m_pMoveCollider->DrawDebugPrimitive({ 1, 1, 1, 1 });
	if (IsPlayer())
	{
		for (const std::pair<uint8_t, Collider*>& attackCollider : m_pColliders)
		{
			if (attackCollider.first == COLLIDER_ID::COL_BODY) continue;

			DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };
			HitResult hit;
			for (Enemy*& enemy : ENEMIES.GetAll())
			{
				for (std::pair<uint8_t, Collider*> enemyCollider : enemy->GetColliders())
				{
					if (attackCollider.second->Collision(enemyCollider.second, {}, hit))
					{
						color = { 0, 0, 1, 1 };
						break;
					}
				}
				if (hit.distance > 0) break;
			}
			attackCollider.second->DrawDebugPrimitive(color);
		}
	}

	if (IsPlayer()) T_GRAPHICS.GetDebugRenderer()->SetSphere(target, 0.1f, { 0, 1, 0, 1 });
#endif // _DEBUG
}

#include "Source/GameObject/Character/Player/State/PlayerCharacterSwordState.h"
void PlayerCharacter::RenderDX12(const RenderContextDX12& rc)
{
	Character::RenderDX12(rc);

	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();
	DirectX::XMFLOAT3 namePos = this->position + DirectX::XMFLOAT3{ 0, 2.2f, 0 };
	float dot = XMFLOAT3Dot(front, namePos - eye);

	LineRendererDX12* dx12_sword = Graphics::Instance().GetLineRendererDX12();
	dx12_sword->Render(rc.d3d_command_list, m_swordSprite.get());
	if (dot < 0.0f) return;

	{
		m_EffectZone->RenderDX12(rc);
		m_EffectCharge->RenderDX12(rc);
		m_EffectHealing->RenderDX12(rc);
	}

#ifdef _DEBUG
	m_pMoveCollider->DrawDebugPrimitive({ 1, 1, 1, 1 });

	if (IsPlayer() && !m_pColliders.empty())
	{
		m_pColliders[COLLIDER_ID::COL_BODY]->DrawDebugPrimitive({ 0, 1, 0, 1 });

		ImVec2 pos = ImGui::GetMainViewport()->Pos;
		ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 450), ImGuiCond_FirstUseEver);

		if (ImGui::Begin("AttackCollider", nullptr, ImGuiWindowFlags_None))
		{
			for (int i = 0; i < 3; i++)
			{
				std::string name = "Attack" + std::to_string(i + 1);
				if (ImGui::TreeNode(name.c_str()))
				{
					float radius = PlayerCharacterState::Sword::sphereAttacks[i].sphere.radius;
					XMFLOAT3 offset = PlayerCharacterState::Sword::sphereAttacks[i].sphere.position;
					float hitStartRate = PlayerCharacterState::Sword::sphereAttacks[i].hitStartRate * 100.0f;	// %表示に
					float hitEndRate = PlayerCharacterState::Sword::sphereAttacks[i].hitEndRate * 100.0f;		// %表示に

					ImGui::InputFloat("radius", &radius);
					ImGui::InputFloat3("offset", &offset.x);
					ImGui::InputFloat("hitStartRate(%)", &hitStartRate);
					if (hitStartRate < 0.0f)	hitStartRate = 0.0f;
					if (hitStartRate > hitEndRate)	hitStartRate = hitEndRate;
					ImGui::InputFloat("hitEndRate(%)", &hitEndRate);
					if (hitEndRate < hitStartRate)	hitEndRate = hitStartRate;
					if (hitEndRate > 100.0f)	hitEndRate = 100.0f;

					PlayerCharacterState::Sword::sphereAttacks[i].sphere.radius = radius;
					PlayerCharacterState::Sword::sphereAttacks[i].sphere.position = offset;
					PlayerCharacterState::Sword::sphereAttacks[i].hitStartRate = hitStartRate * 0.01f;
					PlayerCharacterState::Sword::sphereAttacks[i].hitEndRate = hitEndRate * 0.01f;
					ImGui::TreePop();
				}
			}
			if (ImGui::TreeNode("SkillAttack1"))
			{
				float radius = PlayerCharacterState::Sword::sphereAttacks[3].sphere.radius;
				XMFLOAT3 offset = PlayerCharacterState::Sword::sphereAttacks[3].sphere.position;
				float hitStartRate = PlayerCharacterState::Sword::sphereAttacks[3].hitStartRate * 100.0f;	// %表示に
				float hitEndRate = PlayerCharacterState::Sword::sphereAttacks[3].hitEndRate * 100.0f;		// %表示に

				ImGui::InputFloat("radius", &radius);
				ImGui::InputFloat3("offset", &offset.x);
				ImGui::InputFloat("hitStartRate(%)", &hitStartRate);
				if (hitStartRate < 0.0f)	hitStartRate = 0.0f;
				if (hitStartRate > hitEndRate)	hitStartRate = hitEndRate;
				ImGui::InputFloat("hitEndRate(%)", &hitEndRate);
				if (hitEndRate < hitStartRate)	hitEndRate = hitStartRate;
				if (hitEndRate > 100.0f)	hitEndRate = 100.0f;

				PlayerCharacterState::Sword::sphereAttacks[3].sphere.radius = radius;
				PlayerCharacterState::Sword::sphereAttacks[3].sphere.position = offset;
				PlayerCharacterState::Sword::sphereAttacks[3].hitStartRate = hitStartRate * 0.01f;
				PlayerCharacterState::Sword::sphereAttacks[3].hitEndRate = hitEndRate * 0.01f;
				ImGui::TreePop();
			}
		}
		ImGui::End();

		for (const std::pair<uint8_t, Collider*>& attackCollider : m_pColliders)
		{
			if (attackCollider.first == COLLIDER_ID::COL_BODY) continue;

			DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };
			HitResult hit;
			for (Enemy*& enemy : ENEMIES.GetAll())
			{
				for (std::pair<uint8_t, Collider*> enemyCollider : enemy->GetColliders())
				{
					if (attackCollider.second->Collision(enemyCollider.second, {}, hit))
					{
						color = { 0, 0, 1, 1 };
						break;
					}
				}
				if (hit.distance > 0) break;
			}
			attackCollider.second->DrawDebugPrimitive(color);
		}
	}

	if (IsPlayer())
	{
		// レンダーコンテキスト設定
		RenderContextDX12 rc;
		rc.d3d_command_list = T_GRAPHICS.GetFrameBufferManager()->GetCommandList();

		// 描画
		m_sphere->SetSphere(target, 0.1f, { 0, 1, 0, 1 });
		m_sphere->Render(rc);
	}
#endif // _DEBUG
}

void PlayerCharacter::OnDamage(const uint16_t& damage)
{
	if (hurtCoolTime > 0.0f) return;
	hp -= damage;
	hurtCoolTime = 0.5f;

	TPSCamera.Shake(0.2f, 0.5f);

	//float vx = hit.position.x - position.x;
	//float vz = hit.position.z - position.z;
	//Turn(1.0f, vx, vz, DirectX::XMConvertToRadians(360));
	if (hp <= 0)
	{
		hp = 0;
		stateMachine->ChangeState(static_cast<int>(STATE::DEATH));
	}
	else
	{
		if (!superArmor)
			stateMachine->ChangeState(static_cast<int>(STATE::HURT));
	}
}

bool PlayerCharacter::InputMove(float elapsedTime) {
	// 移動処理
	Move(inputDirection.x, inputDirection.y, this->moveSpeed);
	// 旋回処理
	TurnByInput();

	return(inputDirection.x != 0 && inputDirection.y != 0);//進行ベクトルがゼロベクトルでない場合入力された
}
void PlayerCharacter::FaceToEnemy()
{
	Enemy* closestEnemy = nullptr; // 一番近い敵を記録
	float closestDistanceSq = FLT_MAX;

	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		float distanceSq = XMFLOAT3LengthSq(this->position - enemy->GetPosition());
		if (distanceSq < 4.0f)
		{
			if (InSight(enemy->GetPosition(), 75.f))
			{
				// 今の敵がこれまでの最も近い敵よりも近い場合
				if (distanceSq < closestDistanceSq)
				{
					closestDistanceSq = distanceSq;
					closestEnemy = enemy;

					FaceTo(closestEnemy->GetPosition());
				}
			}
		}
	}
}
void PlayerCharacter::Jump()
{
	if (!isGround) return;
	if (input & Input_Jump)
	{
		Character::Jump(jumpSpeed); // Character::Jump
	}
}

bool PlayerCharacter::InputDodge()
{
	if (input & Input_Dodge)
	{
		DirectX::XMFLOAT2 direction = GetInputDirection();
		//velocity.x = direction.x * dodgeSpeed;
		//velocity.z = direction.y * dodgeSpeed;
		angle.y = atan2f(direction.x, direction.y);
		velocity.x = sinf(angle.y) * dodgeSpeed;
		velocity.z = cosf(angle.y) * dodgeSpeed;

		return true;
	}
	return false;
}

void PlayerCharacter::FaceToCamera()
{
	if (!IsPlayer()) return;
	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	Turn(1.0f, front.x, front.z, turnSpeed);
}

void PlayerCharacter::TurnByInput()
{
	if (!IsPlayer()) return;
	Turn(T_TIMER.Delta(), inputDirection.x, inputDirection.y, turnSpeed);
}

void PlayerCharacter::RecoverMp(float elapsedTime)
{
	ModifyMp(elapsedTime * mpRecoverRate);
}
void PlayerCharacter::ModifyMp(float mp)
{
	this->mp += mp;
	if (this->mp >= maxMp) this->mp = maxMp;
	if (this->mp < 0.0f) this->mp = 0.0f;
}

float PlayerCharacter::GetMpCost(int idx)
{
	if (mpCost.find(idx) != mpCost.end())
	{
		return mpCost[idx];
	}

	return 0.0f;
}

void PlayerCharacter::SkillCost(int idx)
{
	// MP消費
	ModifyMp(-GetMpCost(idx));
	// タイマー
	ResetSkillTimer(idx);
}

// NETWORK
void PlayerCharacter::GetSyncData(SYNC_DATA& data)
{
	std::lock_guard<std::mutex> lock(m_mut);
	data.client_id = m_client_id;
	data.position[0] = position.x;
	data.position[1] = position.y;
	data.position[2] = position.z;
	data.velocity[0] = velocity.x;
	data.velocity[1] = velocity.y;
	data.velocity[2] = velocity.z;
	data.rotate = angle.y;
	data.state = static_cast<uint8_t>(stateMachine->GetStateIndex());
	data.sub_state = static_cast<uint8_t>(stateMachine->GetState()->GetSubStateIndex());
}

void PlayerCharacter::ImportSyncData(const SYNC_DATA& data)
{
	if (m_tempData.old_sync_count < data.sync_count_id)
	{
		std::lock_guard<std::mutex> lock(m_mut);
		Show();
		if (m_tempData.old_sync_count == 0)	// 初めての同期
		{
			SetPosition({ data.position[0], data.position[1], data.position[2] });
			Stop();
			//AddImpulse({ data.velocity[0], data.velocity[1], data.velocity[2] });
			SetAngle({ 0.0f, data.rotate, 0.0f });
		}
		m_tempData.old_sync_count = m_tempData.sync_data.sync_count_id;
		m_tempData.timer = 0.0f;
		m_tempData.position = position;
		m_tempData.angle = angle.y;
		m_tempData.sync_data = data;
		m_tempData.time = (m_tempData.sync_data.sync_count_id - m_tempData.old_sync_count) * 0.25f;

		if (data.velocity[1] > 0.0f)
		{
			position.y = data.position[1];
			velocity.y = data.velocity[1];
		}

		if (data.state != static_cast<uint8_t>(GetStateMachine()->GetStateIndex()))
		{
			GetStateMachine()->ChangeState(data.state);
		}
		else if (data.sub_state != static_cast<uint8_t>(GetStateMachine()->GetState()->GetSubStateIndex()))
		{
			GetStateMachine()->ChangeSubState(data.sub_state);
		}
	}
}