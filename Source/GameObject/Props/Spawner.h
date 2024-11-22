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
    @brief  �G�𐶐�����  
    @par    [����]
*//***************************************************************************/
class Spawner : public GameObject
{
public:
    Spawner();
    ~Spawner() {}

    // �X�V����
    virtual void Update(float elapsedTime) override;

    virtual void Render(const RenderContext& rc)override;

    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

    void SpawnEnemy(float elapsedTime);

    //�v���C���[���G
    bool SearchPlayer();
    float GetSerchRenge() { return serchRange; }
    float GetTerrytoryRange() { return territoryRange; }
    DirectX::XMFLOAT3 GetTerrtoryOrigin() { return territoryOrigin; }
    int CountEnemiesInRange();

private:
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    float territoryRange = 7.0f; 
    float serchRange = 10.f;

    DirectX::XMFLOAT3 playerPosition = {};

    DirectX::XMFLOAT3 spawnPos{};

    int spawnedEnemyCount = 0;             // ���݂܂łɐ������ꂽ�G�l�~�[�̐�
    float spawntimer = 0.f;
    float spawntime = 2.f;
    int maxEnemies = 20;
};
#endif




