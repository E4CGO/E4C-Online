﻿//! @file PlayerCharacterManager.cpp
//! @note 

#include "PlayerCharacterManager.h"
#include <iostream>

/**************************************************************************//**
	@brief		プレイヤーキャラクター更新処理
	@param[in]	elapsedTime 経過時間
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::Update(float elapsedTime)
{
	ObjectManager<PlayerCharacter>::Update(elapsedTime);
}
/**************************************************************************//**
	@brief		プレイヤーキャラクター描画処理
	@param[in]	rc レンダーコンテキスト
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::Render(const RenderContext& rc)
{
	ObjectManager<PlayerCharacter>::Render(rc);
}
/**************************************************************************//**
	@brief		プレイヤーキャラクター描画処理
	@param[in]	rc レンダーコンテキスト
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::RenderDX12(const RenderContextDX12& rc)
{
	ObjectManager<PlayerCharacter>::RenderDX12(rc);
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
PlayerCharacter* PlayerCharacterManager::GetPlayerCharacterById(uint32_t client_id)
{
	std::lock_guard<std::mutex> lock(m_mut);
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
PlayerCharacter* PlayerCharacterManager::UpdatePlayerData(const uint32_t client_id, const char* name, const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM])
{
	PlayerCharacter* player = GetPlayerCharacterById(client_id);
	if (player == nullptr)
	{
		// 新プレイヤー
		player = new PlayerCharacter(client_id, name, appearance);
		player->Hide();
		Register(player);
		player->GetStateMachine()->ChangeState(PlayerCharacter::STATE::IDLE);

		std::cout << "New PlayerCharacter: " << static_cast<int>(client_id) << std::endl;
		return player;
	}
	else
	{
		if (client_id != GAME_DATA.GetClientId())
		{
			// プレイヤーデータ更新
			player->SetName(name);
			player->LoadAppearance(appearance);
			std::cout << "Update PlayerCharacter: " << static_cast<int>(client_id) << std::endl;
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
void PlayerCharacterManager::SyncPlayer(const uint32_t client_id, const PlayerCharacter::SYNC_DATA& data)
{
	PlayerCharacter* player = GetPlayerCharacterById(client_id);
	std::lock_guard<std::mutex> lock(m_mut);
	if (player == nullptr) return;
	// 補間？
	player->ImportSyncData(data);
}


/**************************************************************************//**
	@brief		プレイヤーキャラクターを削除
	@param[in]	client_id クライアントID
	@return		なし
*//***************************************************************************/
void PlayerCharacterManager::Remove(const uint32_t client_id)
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

