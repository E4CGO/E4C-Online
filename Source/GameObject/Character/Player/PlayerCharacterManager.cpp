//! @file PlayerCharacterManager.cpp
//! @note 

#include "PlayerCharacterManager.h"

/**************************************************************************//**
	@brief		プレイヤーキャラクター更新処理
	@param[in]	elapsedTime 経過時間
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::Update(float elapsedTime)
{
	std::lock_guard<std::mutex> lock(m_mut);
	ObjectManager<PlayerCharacter>::Update(elapsedTime);
}

/**************************************************************************//**
	@brief		自分のプレイヤーキャラクターを取得
	@param[in]	なし
	@return		プレイヤーキャラクター参照ポインタ
*//***************************************************************************/
PlayerCharacter* PlayerCharacterManager::GetPlayerCharacterById(void)
{
	return GetPlayerCharacterById(GAME_DATA.GetClientId());
}
/**************************************************************************//**
	@brief		プレイヤーキャラクターを取得
	@param[in]	client_id	クライアントID
	@return		プレイヤーキャラクター参照ポインタ
*//***************************************************************************/
PlayerCharacter* PlayerCharacterManager::GetPlayerCharacterById(uint64_t client_id)
{
	for (PlayerCharacter* player : this->items)
	{
		if (player->GetClientId() == client_id) return player;
	}
	return nullptr;
}

/**************************************************************************//**
	@brief		プレイヤーキャラクターを追加・更新
	@param[in]	client_id	クライアントID
	@param[in]	name		プレイヤー名前
	@param[in]	appearance	プレイヤー外見パターン
	@return		プレイヤーキャラクター参照ポインタ
*//***************************************************************************/
PlayerCharacter* PlayerCharacterManager::UpdatePlayerData(const uint64_t client_id, const char* name, const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM])
{
	std::lock_guard<std::mutex> lock(m_mut);
	PlayerCharacter* player = GetPlayerCharacterById(client_id);
	if (player == nullptr)
	{
		// 新プレイヤー
		player = new PlayerCharacter(client_id, name, appearance);
		player->Hide();
		Register(player);
		player->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);
		return player;
	}
	else
	{
		if (client_id != GAME_DATA.GetClientId())
		{
			// プレイヤーデータ更新
			player->SetName(name);
			player->LoadAppearance(appearance);
		}
		return player;
	}
}

/**************************************************************************//**
	@brief	プレイヤーキャラクターを同期
	@param[in]	client_id クライアントID
	@param[in]	data 同期データ
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::SyncPlayer(const uint64_t client_id, const PlayerCharacter::SYNC_DATA& data)
{
	PlayerCharacter* player = GetPlayerCharacterById(client_id);
	if (player == nullptr) return;
	std::lock_guard<std::mutex> lock(m_mut);
	// 補間？
	player->ImportSyncData(data);
}


/**************************************************************************//**
	@brief		プレイヤーキャラクターを削除
	@param[in]	client_id クライアントID
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::Remove(const uint64_t client_id)
{
	std::lock_guard<std::mutex> lock(m_mut);
	PlayerCharacter* player = GetPlayerCharacterById(client_id);
	if (player != nullptr)
	{
		ObjectManager<PlayerCharacter>::Remove(player);
	}
}

// 自分以外全てのプレイヤーを削除
void PlayerCharacterManager::ClearOtherPlayers()
{
	std::lock_guard<std::mutex> lock(m_mut);
	for (PlayerCharacter* player : this->items)
	{
		if (player->GetClientId() != GAME_DATA.GetClientId())
		{
			delete player;
		}
	}
	std::erase_if(this->items, [this](const auto& item) { return item->GetClientId() != GAME_DATA.GetClientId(); });
}

