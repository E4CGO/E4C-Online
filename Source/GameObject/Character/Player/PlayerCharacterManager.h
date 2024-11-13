﻿//! @file PlayerCharacterManager.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_MANAGER_H__
#define __INCLUDED_PLAYER_CHARACTER_MANAGER_H__

#include <mutex>
#include "GameObject/ObjectManager.h"
#include "PlayerCharacter.h"

/**************************************************************************//**
	@class	PlayerCharacterManager
	@brief	プレイヤーキャラクター管理クラス
	@par	[説明]
			自分を含む全てのプレイヤーを管理
*//***************************************************************************/
class PlayerCharacterManager : public ObjectManager<PlayerCharacter>, public Singleton<PlayerCharacterManager>
{
	friend class Singleton<PlayerCharacterManager>;
private:

	// コンストラクタ
	PlayerCharacterManager() = default;
	// デストラクタ
	~PlayerCharacterManager() = default;
public:
	// オブジェクトとして更新処理
	void Update(float elapsedTime) override;
	// プレイヤーキャラクターを取得
	PlayerCharacter* GetPlayerCharacterById(void);
	PlayerCharacter* GetPlayerCharacterById(uint64_t client_id);
	// プレイヤーキャラクターを追加
	PlayerCharacter* UpdatePlayerData(const uint64_t client_id, const char* name, const uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);
	// プレイヤーキャラクターを同期
	void SyncPlayer(const uint64_t client_id, const PlayerCharacter::SYNC_DATA& data);
	// プレイヤーキャラクターを削除
	void Remove(const uint64_t client_id);

	// 自分以外全てのプレイヤーを削除
	void ClearOtherPlayers();
private:
	uint64_t m_local_client_id = 0; //ローカルプレイヤーID offline = 0;
	std::mutex m_mut;				// ロック
};

#endif // ! __INCLUDED_PLAYER_CHARACTER_H__
