#include "NonPlayerCharacter.h"

#include <profiler.h>
#include <iostream>

#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "GameObject/Character/Player/NonPlayerCharacterState.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

#include "GameData.h"

NonPlayerCharacter::NonPlayerCharacter(uint64_t id, const char* name, uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]) : Character()
{
	moveSpeed = 10.0f;
	turnSpeed = DirectX::XMConvertToRadians(720);
	jumpSpeed = 20.0f;
	dodgeSpeed = 20.0f;

	stateMachine = new StateMachine<NonPlayerCharacter>;
	RegisterCommonState();
	stateMachine->SetState(static_cast<int>(State::Waiting));

	mpCost[static_cast<int>(State::Dodge)] = 20.0f;

	// 衝突判定
	SetCollider(Collider::COLLIDER_TYPE::SPHERE);

	m_client_id = id;
	this->name = name;

	LoadAppearance(appearance);
}

NonPlayerCharacter::NonPlayerCharacter(PlayerCharacterData::CharacterInfo dataInfo) : Character()
{
	moveSpeed = 10.0f;
	turnSpeed = DirectX::XMConvertToRadians(720);
	jumpSpeed = 20.0f;
	dodgeSpeed = 20.0f;

	m_menuVisible = dataInfo.visible;
	std::string m_SaveFile = dataInfo.save;

	stateMachine = new StateMachine<NonPlayerCharacter>;
	RegisterCommonState();
	stateMachine->SetState(static_cast<int>(State::Waiting));

	mpCost[static_cast<int>(State::Dodge)] = 20.0f;

	// 衝突判定
	SetCollider(Collider::COLLIDER_TYPE::SPHERE);

	LoadAppearance(dataInfo.Character.pattern);
}

/**************************************************************************//**
	@brief		外見パターンを読み取る
	@param[in]	appearance
	@return		なし
*//***************************************************************************/
void NonPlayerCharacter::LoadAppearance(uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM])
{
	ModelObject::CleanModels();

	for (uint8_t i = 0; i < PlayerCharacterData::APPEARANCE_PATTERN::NUM; i++)
	{
		PlayerCharacterData::Instance().LoadAppearance(reinterpret_cast<Player*>(this), i, appearance[i]);
	}

	stateMachine->SetState(static_cast<int>(State::Waiting));
}

NonPlayerCharacter::~NonPlayerCharacter()
{
	delete stateMachine;

	for (const std::pair<int, Collider*>& collider : m_pattackColliders)
	{
		delete collider.second;
	}
	m_pattackColliders.clear();
}

void NonPlayerCharacter::RegisterCommonState()
{
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Idle), new NonPlayerCharacterState::IdleState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Move), new NonPlayerCharacterState::MoveState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Jump), new NonPlayerCharacterState::JumpState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Fall), new NonPlayerCharacterState::FallState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Land), new NonPlayerCharacterState::LandState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Dodge), new NonPlayerCharacterState::DodgeState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Hurt), new NonPlayerCharacterState::HurtState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Death), new NonPlayerCharacterState::DeathState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Waiting), new NonPlayerCharacterState::WaitState(this));
	stateMachine->RegisterState(static_cast<int>(NonPlayerCharacter::State::Ready), new NonPlayerCharacterState::ReadyState(this));
}

void NonPlayerCharacter::UpdateTarget()
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

void NonPlayerCharacter::UpdateColliders()
{
	collider->SetPosition(position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 } *scale);
	collider->SetScale(DirectX::XMFLOAT3{ height * 0.3f, height * 0.3f, height * 0.3f } *scale);
}

bool  NonPlayerCharacter::CollisionVsEnemies(Collider* collider, int damage, bool power, float force, int effectIdx, float effectScale)
{
	bool isHit = false;
	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		if (enemy->GetHurtCoolTime() > 0.0f) continue; // 無敵時間
		for (std::pair<int, Collider*>enemyCollider : enemy->GetColliders())
		{
			HitResult hit;
			if (collider->Collision(enemyCollider.second, {}, hit))
			{
				ENEMYCOLLISIONS.Register(enemy, enemyCollider.first, damage, power, hit.normal * -force);
				if (effectIdx > -1)
				{
					EFFECTS.GetEffect(effectIdx)->Play(hit.position, effectScale);
				}

				enemy->SetHurtCoolTime(0.4f);
				isHit = true;
				std::cout << "Hit enemy_id: " << enemy->GetEnemyId() << std::endl;
				break;
			}
		}
	}
	return isHit;
}

void NonPlayerCharacter::UpdateInput()
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
		if (T_INPUT.KeyDown(VK_CONTROL) && mp >= GetMpCost(static_cast<int>(State::Dodge))) input |= Input_Dodge;

		if (T_INPUT.KeyPress(VK_LBUTTON) && !TentacleLib::isShowCursor()) input |= Input_Attack_N;
		if (T_INPUT.KeyPress(VK_RBUTTON) && !TentacleLib::isShowCursor()) input |= Input_Attack_S;

		if (
			T_INPUT.KeyPress('1')
			&& mp >= GetMpCost(static_cast<int>(State::Skill_1))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_1)) == 0.0f
			) input |= Input_Skill_1;

		if (
			T_INPUT.KeyPress('2')
			&& mp >= GetMpCost(static_cast<int>(State::Skill_2))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_2)) == 0.0f
			) input |= Input_Skill_2;

		if (
			T_INPUT.KeyPress('3')
			&& mp >= GetMpCost(static_cast<int>(State::Skill_3))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_3)) == 0.0f
			) input |= Input_Skill_3;

		if (
			T_INPUT.KeyPress('4')
			&& mp >= GetMpCost(static_cast<int>(State::Skill_4))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_4)) == 0.0f
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
		if (T_INPUT.GamePadKeyDown(GAME_PAD_BTN::B) && mp >= GetMpCost(static_cast<int>(State::Dodge))) input |= Input_Dodge;

		if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::X) && !TentacleLib::isShowCursor()) input |= Input_Attack_N;
		if (T_INPUT.GamePadKeyPress(GAME_PAD_BTN::LTRIGGER) && !TentacleLib::isShowCursor()) input |= Input_Attack_S;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::LSHOULDER)
			&& mp >= GetMpCost(static_cast<int>(State::Skill_1))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_1)) == 0.0f
			) input |= Input_Skill_1;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::RSHOULDER)
			&& mp >= GetMpCost(static_cast<int>(State::Skill_2))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_2)) == 0.0f
			) input |= Input_Skill_2;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::RTRIGGER)
			&& mp >= GetMpCost(static_cast<int>(State::Skill_3))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_3)) == 0.0f
			) input |= Input_Skill_3;

		if (
			T_INPUT.GamePadKeyPress(GAME_PAD_BTN::Y)
			&& mp >= GetMpCost(static_cast<int>(State::Skill_4))
			&& GetSkillTimerTime(static_cast<int>(State::Skill_4)) == 0.0f
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
DirectX::XMFLOAT2 NonPlayerCharacter::GetInputDirection()
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
float NonPlayerCharacter::GetSkillTimerTime(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer;
}
float NonPlayerCharacter::GetSkillTimerRate(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer / skillTimer[idx].time;
}
void NonPlayerCharacter::ResetSkillTimer(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return;
	skillTimer[idx].currentTimer = skillTimer[idx].time;
}
void NonPlayerCharacter::UpdateSkillTimers(float elapsedTime)
{
	for (std::pair<int, SkillTimer> timer : skillTimer)
	{
		skillTimer[timer.first].currentTimer -= elapsedTime;
		if (skillTimer[timer.first].currentTimer <= 0.0f) skillTimer[timer.first].currentTimer = 0.0f;
	}
}

void NonPlayerCharacter::Update(float elapsedTime)
{
	{
		ProfileScopedSection_2("input", ImGuiControl::Profiler::Red);
		if (IsPlayer()) // 自機限定 ステート管理
		{
			input = 0;
			UpdateTarget();
			UpdateInput();
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
		ProfileScopedSection_2("character", ImGuiControl::Profiler::Purple);
		Character::Update(elapsedTime);
	}
}

void NonPlayerCharacter::Render(const RenderContext& rc)
{
	Character::Render(rc);

	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();
	DirectX::XMFLOAT3 namePos = this->position + DirectX::XMFLOAT3{ 0, 2.2f, 0 };
	float dot = XMFLOAT3Dot(front, namePos - eye);
	if (dot < 0.0f) return;

	// 名前表示
	DirectX::XMFLOAT3 pos = T_GRAPHICS.GetScreenPosition(namePos);
	T_TEXT.Render(
		FONT_ID::HGpop,
		name.c_str(),
		pos.x, pos.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM,
		0.5f,
		1
	);

#ifdef _DEBUG
	collider->DrawDebugPrimitive({ 1, 1, 1, 1 });
	if (IsPlayer())
	{
		for (const std::pair<int, Collider*>& attackCollider : m_pattackColliders)
		{
			DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };
			HitResult hit;
			for (Enemy*& enemy : ENEMIES.GetAll())
			{
				for (std::pair<int, Collider*> enemyCollider : enemy->GetColliders())
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

	if (IsPlayer()) T_GRAPHICS.GetDebugRenderer()->DrawSphere(target, 0.1f, { 0, 1, 0, 1 });
#endif // _DEBUG
}

void NonPlayerCharacter::OnDamage(const HitResult& hit, int damage)
{
	if (hurtCoolTime > 0.0f) return;
	hp -= damage;
	hurtCoolTime = 0.5f;

	TPSCamera.Shake(0.2f, 0.5f);

	float vx = hit.position.x - position.x;
	float vz = hit.position.z - position.z;
	Turn(1.0f, vx, vz, DirectX::XMConvertToRadians(360));
	if (hp <= 0)
	{
		hp = 0;
		stateMachine->ChangeState(static_cast<int>(State::Death));
	}
	else
	{
		stateMachine->ChangeState(static_cast<int>(State::Hurt));
	}
}

void NonPlayerCharacter::InputMove(float elapsedTime) {
	if (inputDirection.x == 0 && inputDirection.y == 0) return; // 方向入力なし

	// 移動処理
	Move(inputDirection.x, inputDirection.y, this->moveSpeed);
	// 旋回処理
	TurnByInput();
}

void NonPlayerCharacter::Jump()
{
	if (!isGround) return;
	if (input & Input_Jump)
	{
		Character::Jump(jumpSpeed); // Character::Jump
	}
}

bool NonPlayerCharacter::InputDodge()
{
	if (input & Input_Dodge)
	{
		DirectX::XMFLOAT2 direction = GetInputDirection();
		velocity.x = direction.x * dodgeSpeed;
		velocity.z = direction.y * dodgeSpeed;
		angle.y = atan2f(direction.x, direction.y);

		return true;
	}
	return false;
}

void NonPlayerCharacter::FaceToCamera()
{
	if (!IsPlayer()) return;
	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	Turn(1.0f, front.x, front.z, turnSpeed);
}

void NonPlayerCharacter::TurnByInput()
{
	if (!IsPlayer()) return;
	Turn(T_TIMER.Delta(), inputDirection.x, inputDirection.y, turnSpeed);
}

void NonPlayerCharacter::RecoverMp(float elapsedTime)
{
	ModifyMp(elapsedTime * mpRecoverRate);
}
void NonPlayerCharacter::ModifyMp(float mp)
{
	this->mp += mp;
	if (this->mp >= maxMp) this->mp = maxMp;
	if (this->mp < 0.0f) this->mp = 0.0f;
}

float NonPlayerCharacter::GetMpCost(int idx)
{
	if (mpCost.find(idx) != mpCost.end())
	{
		return mpCost[idx];
	}

	return 0.0f;
}

void NonPlayerCharacter::SkillCost(int idx)
{
	// MP消費
	ModifyMp(-GetMpCost(idx));
	// タイマー
	ResetSkillTimer(idx);
}