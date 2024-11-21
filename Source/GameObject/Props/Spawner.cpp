#include "Spawner.h"
#include "GameObject/Character/Player/PlayerManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/Tool/Mathf.h"
#include "Source/Scene/Stage/TestingStage.h"
#include <GameObject/Character/Enemy/EnemyManager.h>
/**************************************************************************//**
     @brief  �R���X�g���N�^  
*//***************************************************************************/
Spawner::Spawner()
{
	
}
/**************************************************************************//**
     @brief    �X�V
    @param[in]    elapsedTime
*//***************************************************************************/
void Spawner::Update(float elapsedTime)
{
	if (SearchPlayer())
	{
		STAGES.GetStage()->SetPhase(TestingStage::PHASE::MONSTERRUSH);
	}
}
/**************************************************************************//**
     @brief    �e���g���[�z�u
    @param[in]    origin
    @param[in]    range
*//***************************************************************************/
void Spawner::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
/**************************************************************************//**
     @brief    �v���C���[����
    @return    
*//***************************************************************************/
bool Spawner::SearchPlayer()
{
	for (Player*& player : PLAYERS.GetAll())
	{
		// �v���C���[�̈ʒu���擾
		playerPosition = player->GetPosition();

		// ���g�ƃv���C���[�̊Ԃ̋������v�Z
		float vx = playerPosition.x - position.x;
		float vy = playerPosition.y - position.y;
		float vz = playerPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);

		// ���m�͈͓����m�F
		if (dist < serchRange)
		{
			return true;
			
		}
		return false;
	}
}
/**************************************************************************//**
     @brief    �`��
    @param[in]    rc
*//***************************************************************************/
void Spawner::Render(const RenderContext& rc)
{

}
