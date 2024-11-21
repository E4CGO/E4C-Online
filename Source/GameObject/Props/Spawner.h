//! @file Spawner.h
//! @note 
#ifndef __INCLUDED_SPAWNER_H__
#define __INCLUDED_SPAWNER_H__
#include "Source/GameObject/GameObject.h"
#include "Source/Scene/Stage/Stage.h"
#include "Source/GameObject/Character/Enemy/Enemy.h"
#include "TAKOEngine/Rendering/Model/ModelDX11.h"
#include "TAKOEngine/Rendering/Model/NewModelDX11.h"
#include "TAKOEngine/Rendering/Model/ModelDX12.h"


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

    virtual void Render(const RenderContext& rc)override;

    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

   

    //プレイヤー索敵
    bool SearchPlayer();

    float GetSerchRenge() { return serchRange; }
    float GetTerrytoryRange() { return territoryRange; }
    DirectX::XMFLOAT3 GetTerrtoryOrigin() { return territoryOrigin; }

private:
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 5.0f; 
    float serchRange = 5.f;

    DirectX::XMFLOAT3 playerPosition = {};
};
#endif




