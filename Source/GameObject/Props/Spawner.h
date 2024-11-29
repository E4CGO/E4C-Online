//! @file Spawner.h
//! @note 
#ifndef __INCLUDED_SPAWNER_H__
#define __INCLUDED_SPAWNER_H__
#include "Source/GameObject/GameObject.h"
#include "Source/Scene/Stage/Stage.h"
#include "Source/GameObject/Character/Enemy/Enemy.h"
#include "Source/GameObject/Character/Enemy/SkeletonMinion.h"
#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"
#include <vector>
#include <algorithm>





/**************************************************************************//**
    @class  Spawner
    @brief  敵を生成する  
    @par    [説明]
*//***************************************************************************/
class Spawner : public GameObject
{
public:
    Spawner();
    ~Spawner() {}

    // 更新処理
    virtual void Update(float elapsedTime) override;
    //描画実行
    virtual void Render(const RenderContext& rc)override;
    //テリトリー設置
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
    //エネミースポーン
    void SpawnEnemy(float elapsedTime);
    //プレイヤー索敵
    bool SearchPlayer();
    //サーチレンジ取得
    float GetSerchRenge() { return serchRange; }
    //テリトリーレンジ取得
    float GetTerrytoryRange() { return territoryRange; }
    //テリトリーオリジン取得
    DirectX::XMFLOAT3 GetTerrtoryOrigin() { return territoryOrigin; }
    //生成するエネミーの数管理
    int CountEnemiesInRange();
private:
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 7.0f; 
    //スポナーがプレイヤーを見つけるレンジ
    float serchRange;
    //エネミー数を見つけるレンジ
    float enemyCountRange=FLT_MAX;

    DirectX::XMFLOAT3 playerPosition = {};

    DirectX::XMFLOAT3 spawnPos{};

    int spawnedEnemyCount = 0;             // 現在までに生成されたエネミーの数
    float spawntimer = 0.f;
    float spawntime = 2.f;
    int maxEnemies = 20;
};
#endif




