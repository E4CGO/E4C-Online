#pragma once

#include "TAKOEngine/AI/StateMachine.h"

#include <memory>
#include <unordered_map>

#include "GameObject/Character/Player/PlayerCharacter.h"
#include "GameData.h"
#include "PlayerCharacterData.h"

class Enemy;

class NonPlayerCharacter : public PlayerCharacter
{
public:
	// コンストラクタ(引数付き)
	NonPlayerCharacter(uint32_t id, const char* name, uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]);
	// コンストラクタ(引数付き)
	NonPlayerCharacter(PlayerCharacterData::CharacterInfo dataInfo);
	// デストラクタ
	~NonPlayerCharacter() = default;
};
