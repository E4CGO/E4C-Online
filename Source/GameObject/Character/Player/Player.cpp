#include "Player.h"

#include <iostream>

#include "TAKOEngine/Physics/SphereCollider.h"
#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player/PlayerState.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

#include "Map\MapTileManager.h"

#include "GameData.h"

Player::Player(const char* filename, float scaling) : Character(filename, scaling)
{
	moveSpeed = 10.0f;
	turnSpeed = DirectX::XMConvertToRadians(720);
	jumpSpeed = 20.0f;
	dodgeSpeed = 20.0f;

	stateMachine = new StateMachine<Player>;
	RegisterCommonState();
	stateMachine->SetState(static_cast<int>(State::Waiting));

	mpCost[static_cast<int>(State::Dodge)] = 20.0f;

	// 衝突判定
	SetCollider(Collider::COLLIDER_TYPE::SPHERE);
}

Player::~Player()
{
	delete stateMachine;

	for (const std::pair<int, Collider*>& collider : attackColliders)
	{
		delete collider.second;
	}
	attackColliders.clear();
}

void Player::RegisterCommonState()
{
	stateMachine->RegisterState(static_cast<int>(State::Idle), new PlayerState::IdleState(this));
	stateMachine->RegisterState(static_cast<int>(State::Move), new PlayerState::MoveState(this));
	stateMachine->RegisterState(static_cast<int>(State::Jump), new PlayerState::JumpState(this));
	stateMachine->RegisterState(static_cast<int>(State::Fall), new PlayerState::FallState(this));
	stateMachine->RegisterState(static_cast<int>(State::Land), new PlayerState::LandState(this));
	stateMachine->RegisterState(static_cast<int>(State::Dodge), new PlayerState::DodgeState(this));
	stateMachine->RegisterState(static_cast<int>(State::Hurt), new PlayerState::HurtState(this));
	stateMachine->RegisterState(static_cast<int>(State::Death), new PlayerState::DeathState(this));
	stateMachine->RegisterState(static_cast<int>(State::Waiting), new PlayerState::WaitState(this));
	stateMachine->RegisterState(static_cast<int>(State::Ready), new PlayerState::ReadyState(this));
}

void Player::UpdateTarget()
{
	// レイの開始位置は足元より少し上
	DirectX::XMFLOAT3 start = Camera::Instance().GetEye();
	// レイの終点位置は移動後の位置
	DirectX::XMFLOAT3 end = Camera::Instance().GetFront() * 100.0f + start;
	// レイキャストによる地面判定
	HitResult hit;
	if (ENEMIES.RayCast(start, end, hit))
	{
		target = hit.position;
	}
	else if (MAPTILES.RayCast(start, end, hit))
	{
		target = hit.position;
	}
	else
	{
		target = end;
	}
}

void Player::UpdateColliders()
{
	collider->SetPosition(position + DirectX::XMFLOAT3{ 0, height * 0.5f, 0 } *scale);
	collider->SetScale(DirectX::XMFLOAT3{ height * 0.3f, height * 0.3f, height * 0.3f } *scale);
}

bool  Player::CollisionVsEnemies(Collider* collider, int damage, bool power, float force, int effectIdx, float effectScale)
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

void Player::UpdateInput()
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
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

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
DirectX::XMFLOAT2 Player::GetInputDirection()
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
float Player::GetSkillTimerTime(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer;
}
float Player::GetSkillTimerRate(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return 0.0f; // デフォルト
	return skillTimer[idx].currentTimer / skillTimer[idx].time;
}
void Player::ResetSkillTimer(int idx)
{
	if (skillTimer.find(idx) == skillTimer.end()) return;
	skillTimer[idx].currentTimer = skillTimer[idx].time;
}
void Player::UpdateSkillTimers(float elapsedTime)
{
	for (std::pair<int, SkillTimer> timer : skillTimer)
	{
		skillTimer[timer.first].currentTimer -= elapsedTime;
		if (skillTimer[timer.first].currentTimer <= 0.0f) skillTimer[timer.first].currentTimer = 0.0f;
	}
}

void Player::Update(float elapsedTime)
{
	if (IsPlayer()) // 自機限定 ステート管理
	{
		input = 0;
		UpdateTarget();
		UpdateInput();
	}
	stateMachine->Update(elapsedTime);

	UpdateSkillTimers(elapsedTime);

	Character::Update(elapsedTime);
}

void Player::Render(const RenderContext& rc)
{
	Character::Render(rc);

	DirectX::XMFLOAT3 front = Camera::Instance().GetFront();
	DirectX::XMFLOAT3 eye = Camera::Instance().GetEye();
	DirectX::XMFLOAT3 namePos = this->position + DirectX::XMFLOAT3{ 0, 2.2f, 0 };
	float dot = XMFLOAT3Dot(front, namePos - eye);
	if (dot < 0.0f) return;

	// 名前表示
	DirectX::XMFLOAT3 pos = T_GRAPHICS.GetScreenPosition(namePos);
	T_TEXT.Render(
		FONT_ID::HGpop,
		name,
		pos.x, pos.y,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM,
		0.5f,
		1
	);

#ifdef _DEBUG
	collider->DrawDebugPrimitive({ 1, 1, 1, 1 });
	if (client_id == GAME_DATA.GetClientId())
	{
		for (const std::pair<int, Collider*>& attackCollider : attackColliders)
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

void Player::OnDamage(const HitResult& hit, int damage)
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

void Player::InputMove(float elapsedTime) {
	if (inputDirection.x == 0 && inputDirection.y == 0) return; // 方向入力なし

	// 移動処理
	Move(inputDirection.x, inputDirection.y, this->moveSpeed);
	// 旋回処理
	TurnByInput();
}

void Player::Jump()
{
	if (!isGround) return;
	if (input & Input_Jump)
	{
		Character::Jump(jumpSpeed); // Character::Jump
	}
}

bool Player::InputDodge()
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

void Player::FaceToCamera()
{
	if (!IsPlayer()) return;
	DirectX::XMFLOAT3 front = Camera::Instance().GetFront();
	Turn(1.0f, front.x, front.z, turnSpeed);
}

void Player::TurnByInput()
{
	if (!IsPlayer()) return;
	Turn(T_TIMER.Delta(), inputDirection.x, inputDirection.y, turnSpeed);
}

void Player::RecoverMp(float elapsedTime)
{
	ModifyMp(elapsedTime * mpRecoverRate);
}
void Player::ModifyMp(float mp)
{
	this->mp += mp;
	if (this->mp >= maxMp) this->mp = maxMp;
	if (this->mp < 0.0f) this->mp = 0.0f;
}

float Player::GetMpCost(int idx)
{
	if (mpCost.find(idx) != mpCost.end())
	{
		return mpCost[idx];
	}

	return 0.0f;
}

void Player::SkillCost(int idx)
{
	// MP消費
	ModifyMp(-GetMpCost(idx));
	// タイマー
	ResetSkillTimer(idx);
}

// Network
void Player::ImportData(PLAYER_DATA data)
{
	if (IsPlayer()) return; // 自機キャラは更新しない
	position = data.position;
	velocity = data.velocity;
	target = data.target;
	angle.y = data.angle;
	color = data.color;
	hp = data.hp;
	maxHp = data.hp;
	if (stateMachine->GetStateIndex() != data.state)
	{
		stateMachine->ChangeState(data.state);
	}
	if (stateMachine->GetState()->GetSubStateIndex() != data.subState)
	{
		stateMachine->ChangeSubState(data.subState);
	}
}
void Player::ExportData(PLAYER_DATA& data)
{
	data.client_id = client_id;
	data.position = position;
	data.velocity = velocity;
	data.target = target;
	data.color = color;
	data.angle = angle.y;
	data.state = stateMachine->GetStateIndex();
	data.hp = hp;
	data.maxHp = hp;
	data.subState = stateMachine->GetState()->GetSubStateIndex();
}