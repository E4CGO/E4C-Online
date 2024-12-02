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
    virtual void RenderDX12(const RenderContextDX12& rc)override;

    //テリトリー設置
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
    //エネミースポーン
    void SpawnEnemy(float elapsedTime,const std::string&enemyType,int maxEnemies,int activeEnemies);
    //スポナーのエネミー管理
    void CleanupEnemies();
    //プレイヤー索敵
    bool SearchPlayer();
    //サーチレンジ取得
    float GetSerchRenge() { return serchRange; }
    //テリトリーレンジ取得
    float GetTerrytoryRange() { return territoryRange; }
    //テリトリーオリジン取得
    DirectX::XMFLOAT3 GetTerrtoryOrigin() { return territoryOrigin; }
    //最大生成数 (Setter)
    int SetMaxEnemies() { return maxEnemies; }
    //同時存在していいエネミーの数 (Setter)
    int SetActiveEnemies() { return activeEnemies; }
    //現在存在数(Getter)
    int GetCurrentAliveCount() { return currentAliveCount; }
    //生成するエネミーの種類(Setter)
    Enemy* SetEnemy(const std::string& enemyType);
private:
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 7.0f; 
    //スポナーがプレイヤーを見つけるレンジ
    float serchRange;
    DirectX::XMFLOAT3 playerPosition = {};

    DirectX::XMFLOAT3 spawnPos{};

    int spawnedEnemyCount=0;  // 現在までに生成されたエネミーの数
    float spawntimer=0;  //タイム更新
    float spawntime=2;   //エネミー生成するまでの秒数
    int maxEnemies;    //最大生成数
    int activeEnemies; //同時存在していいエネミーの数

    std::vector<Enemy*> spawnedEnemies; // スポナーが管理するリスト

    int currentAliveCount;//今生存してるエネミー数
};
#endif




