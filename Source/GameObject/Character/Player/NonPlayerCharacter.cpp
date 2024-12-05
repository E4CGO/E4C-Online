#include "NonPlayerCharacter.h"

#include <profiler.h>
#include <iostream>

#include "TAKOEngine/Physics/CollisionDataManager.h"
#include "TAKOEngine/Effects/EffectManager.h"
#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "GameObject/Character/Player/State/NonPlayerCharacterState.h"
#include "GameObject/Character/Enemy/EnemyManager.h"

#include "GameData.h"

NonPlayerCharacter::NonPlayerCharacter(uint32_t id, const char* name, uint8_t appearance[PlayerCharacterData::APPEARANCE_PATTERN::NUM]) : PlayerCharacter(id, name, appearance)
{
}

NonPlayerCharacter::NonPlayerCharacter(PlayerCharacterData::CharacterInfo dataInfo) : PlayerCharacter(dataInfo)
{
}
