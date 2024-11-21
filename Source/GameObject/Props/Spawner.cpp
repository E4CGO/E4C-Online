#include "Spawner.h"
#include "GameObject/Character/Player/PlayerManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/Tool/Mathf.h"
#include "Source/Scene/Stage/TestingStage.h"
#include <GameObject/Character/Enemy/EnemyManager.h>
/**************************************************************************//**
     @brief  コンストラクタ  
*//***************************************************************************/
Spawner::Spawner()
{
	
}
/**************************************************************************//**
     @brief    更新
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
     @brief    テリトリー配置
    @param[in]    origin
    @param[in]    range
*//***************************************************************************/
void Spawner::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}
/**************************************************************************//**
     @brief    プレイヤー発見
    @return    
*//***************************************************************************/
bool Spawner::SearchPlayer()
{
	for (Player*& player : PLAYERS.GetAll())
	{
		// プレイヤーの位置を取得
		playerPosition = player->GetPosition();

		// 自身とプレイヤーの間の距離を計算
		float vx = playerPosition.x - position.x;
		float vy = playerPosition.y - position.y;
		float vz = playerPosition.z - position.z;
		float dist = sqrtf(vx * vx + vy * vy + vz * vz);

		// 検知範囲内か確認
		if (dist < serchRange)
		{
			return true;
			
		}
		return false;
	}
}
/**************************************************************************//**
     @brief    描画
    @param[in]    rc
*//***************************************************************************/
void Spawner::Render(const RenderContext& rc)
{

}
