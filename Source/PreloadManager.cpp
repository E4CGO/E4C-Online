//! @file PreloadManager.cpp
//! @note
//! 
#include "PreloadManager.h"

/**************************************************************************//**
	@brief	コンストラクタ
*//***************************************************************************/
PreloadManager::PreloadManager()
{
	{
		//NewGroup("SceneLoading.Sprite", SPRITE);
		//m_loadList["SceneLoading.Sprite"].list = {
		//	"Data/Sprites/gear.png"
		//};
	}

	{
		//NewGroup("SceneTitle.Sprite", SPRITE);
		//m_loadList["SceneTitle.Sprite"].list = {
		//	"",
		//	"Data/Sprites/UI/Title/background.png",
		//	"Data/Sprites/UI/Title/title_logo.png",
		//	"Data/Sprites/UI/Title/cont_d.png",
		//	"Data/Sprites/UI/Title/cont_g.png",
		//	"Data/Sprites/UI/Title/cont_h.png",
		//	"Data/Sprites/UI/Title/credits_d.png",
		//	"Data/Sprites/UI/Title/credits_h.png",
		//	"Data/Sprites/UI/Title/exit_d.png",
		//	"Data/Sprites/UI/Title/exit_h.png",
		//	"Data/Sprites/UI/Title/new_d.png",
		//	"Data/Sprites/UI/Title/new_h.png",
		//	"Data/Sprites/UI/Title/option_d.png",
		//	"Data/Sprites/UI/Title/option_h.png",
		//};
	}

	{
		//NewGroup("SceneCharacter.Sprite", SPRITE);
		//m_loadList["CharacterUISprite"].list = {
		//	"Data/Sprites/UI/start.png",
		//	"Data/Sprites/UI/back.png",
		//	"Data/Sprites/UI/keybinds/key_Left.png",
		//	"Data/Sprites/UI/keybinds/key_Right.png",
		//	"Data/Sprites/big_background.t.png",
		//};
	}

	{
		NewGroup("CharacterModels", MODEL);
		m_loadList["CharacterModels"].list = {
			//男性よう
			"Data/Model/Character/PlayerModels/PlayerMale/MDLANM_PLAYER_BODY.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR2.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Hair/MDLANM_PLAYER_HAIR3.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_ARM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_ARM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_ARM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume1/MDLANM_PLAYER_SHOES.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume2/MDLANM_PLAYER_SHOES.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Costume3/MDLANM_PLAYER_SHOES.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD2.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_SWORD3.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD.glb",
			"Data/Model/Character/PlayerModels/PlayerMale/Weapons/MDLANM_PLAYER_ROD2.glb",
			// 女
			"Data/Model/Character/PlayerModels/PlayerFemale/MDLANM_PLAYER_F_BODY.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR2.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR3.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Hair/MDLANM_PLAYER_F_HAIR4.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_TOP.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/MDLANM_PLAYER_F_ARM.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume3/MDLANM_PLAYER_F_BOTTOM.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume1/MDLANM_PLAYER_F_SHOES.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Costume2/MDLANM_PLAYER_F_SHOES.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD2.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_SWORD3.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD.glb",
			"Data/Model/Character/PlayerModels/PlayerFemale/Weapons/MDLANM_PLAYER_F_ROD2.glb",
		};
	}

	{
		NewGroup("OpenWorldModels", MODEL);
		m_loadList["OpenWorldModels"].list = {
			"Data/Model/Stage/Terrain_Collision.glb",
			"Data/Model/Stage/Terrain_Village_Collision.glb",
			"Data/Model/Stage/Terrain_Map.glb",
			"Data/Model/Stage/Terrain_Village.glb",
			"Data/Model/Stage/Terrain_Bush.glb",
			"Data/Model/Stage/Terrain_Tree_1_Trunk.glb",
			"Data/Model/Stage/Terrain_Tree_1_Leaf.glb",
			"Data/Model/Stage/Terrain_Tree_2_Trunk.glb",
			"Data/Model/Stage/Terrain_Tree_2_Leaf.glb",
			"Data/Model/Stage/Terrain_Flower.glb",
			"Data/Model/Stage/Terrain_Portal.glb",
			"Data/Model/Object/BlockTarget.glb",
			"Data/Model/Object/CloseTarget1.glb",
			"Data/Model/Object/CloseTarget2.glb",
			"Data/Model/Cube/Cube.fbx",
		};
	}

	{
		NewGroup("DungeonModels", MODEL);
		m_loadList["DungeonModels"].list = {
			"Data/Model/DungeonAssets/FloorCollision_01a.glb",
			"Data/Model/DungeonAssets/FountainCollision.glb",
			"Data/Model/DungeonAssets/SlopeCollision_01a.glb",
			"Data/Model/DungeonAssets/SM_Arch_01a.glb",
			"Data/Model/DungeonAssets/SM_Arch_Entrance_01a.glb",
			"Data/Model/DungeonAssets/SM_Arch_Floor_01a.glb",
			"Data/Model/DungeonAssets/SM_Barrel.glb",
			"Data/Model/DungeonAssets/SM_BossRoom.glb",
			"Data/Model/DungeonAssets/SM_Brick_Wall_01a.glb",
			"Data/Model/DungeonAssets/SM_Caramel_01.glb",
			"Data/Model/DungeonAssets/SM_Caramel_02.glb",
			"Data/Model/DungeonAssets/SM_Chandelier.glb",
			"Data/Model/DungeonAssets/SM_Cloud_01.glb",
			"Data/Model/DungeonAssets/SM_Cloud_02.glb",
			"Data/Model/DungeonAssets/SM_Cream_01.glb",
			"Data/Model/DungeonAssets/SM_Cream_02.glb",
			"Data/Model/DungeonAssets/SM_Fire_Hydrant.glb",
			"Data/Model/DungeonAssets/SM_Floor_01a.glb",
			"Data/Model/DungeonAssets/SM_Floor_01b.glb",
			"Data/Model/DungeonAssets/SM_Floor_02a.glb",
			"Data/Model/DungeonAssets/SM_Floor_03a.glb",
			"Data/Model/DungeonAssets/SM_Floor_Brown.glb",
			"Data/Model/DungeonAssets/SM_Floor_Check_Board.glb",
			"Data/Model/DungeonAssets/SM_Floor_Cloud_01a.glb",
			"Data/Model/DungeonAssets/SM_Floor_Gray.glb",
			"Data/Model/DungeonAssets/SM_Fountain_01.glb",
			"Data/Model/DungeonAssets/SM_Heart_Arch_01a.glb",
			"Data/Model/DungeonAssets/SM_Lollipop_01a.glb",
			"Data/Model/DungeonAssets/SM_Lollipop_01b.glb",
			"Data/Model/DungeonAssets/SM_Lollipop_02a.glb",
			"Data/Model/DungeonAssets/SM_Pillar_01a.glb",
			"Data/Model/DungeonAssets/SM_Pillar_01b.glb",
			"Data/Model/DungeonAssets/SM_Pillar_Base_01a.glb",
			"Data/Model/DungeonAssets/SM_Pillar_Top_01a.glb",
			"Data/Model/DungeonAssets/SM_Plastic_Flower.glb",
			"Data/Model/DungeonAssets/SM_Set_01a.glb",
			"Data/Model/DungeonAssets/SM_Set_01b.glb",
			"Data/Model/DungeonAssets/SM_Set_01c.glb",
			"Data/Model/DungeonAssets/SM_Stairs_Railing_01a.glb",
			"Data/Model/DungeonAssets/SM_Stairs_Steps_01a.glb",
			"Data/Model/DungeonAssets/SM_Stairs_Steps_01b.glb",
			"Data/Model/DungeonAssets/SM_Star.glb",
			"Data/Model/DungeonAssets/SM_Toy_Arch_01a.glb",
			"Data/Model/DungeonAssets/SM_Toy_Arch_Support_01a.glb",
			"Data/Model/DungeonAssets/SM_Toy_Arch_Top_01a.glb",
			"Data/Model/DungeonAssets/SM_Wall_Cloud.glb",
			"Data/Model/DungeonAssets/SM_Wall_Paper.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_01a.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_01b.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_02a.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_02b.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_03a.glb",
			"Data/Model/DungeonAssets/SM_Wall_Pattern_04a.glb",
			"Data/Model/DungeonAssets/SM_Wall_Squares.glb",
			"Data/Model/DungeonAssets/SM_Well_01a.glb",
			"Data/Model/DungeonAssets/WallCollision_01a.glb",
			"Data/Model/DungeonAssets/WallCollision_01b.glb",
		};
	}

	{
		NewGroup("EnemyModels", MODEL);
		m_loadList["EnemyModels"].list = {
			"Data/Model/Enemy/MDLANM_ENMbird_0120.glb",
			"Data/Model/Enemy/MDLANM_ENMboss_0123.glb",
			"Data/Model/Enemy/MDLANM_ENMboss_BossElectircCircre.glb",
			"Data/Model/Enemy/MDLANM_ENMmouse_0117.glb",
			"Data/Model/Enemy/MDLANM_ENMpig_0120.glb",
			"Data/Model/Enemy/MDLANM_ENMslime_0121.glb",
		};
	}
}

/**************************************************************************//**
	@brief	ロードスレッド生成
*//***************************************************************************/
void PreloadManager::Load()
{
	for (const std::string& group : m_groupOrder)
	{
		m_loadList[group].thread = std::thread(&PreloadManager::LoadThread, this, group);
	}
}
/**************************************************************************//**
	@brief		ロードスレッド関数
	@param[in]	group ロードグループネーム
*//***************************************************************************/
void PreloadManager::LoadThread(const std::string& group)
{
	if (m_loadList.contains(group))
	{
		m_mtLoading.lock();

		switch (m_loadList[group].type)
		{
		case PreloadManager::SPRITE:
			CoInitialize(nullptr);
			for (const std::string filename : m_loadList[group].list)
			{
				m_spriteD12List.insert(RESOURCE.LoadSpriteResourceDX12(filename.c_str()));
			}
			CoUninitialize();
			break;
		case PreloadManager::MODEL:
			for (const std::string filename : m_loadList[group].list)
			{
				m_modelList.insert(RESOURCE.LoadModelDX12Resource(filename.c_str()));
			}
			break;
		}
		m_mtLoading.unlock();
		m_loadList[group];
	}
}

/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
PreloadManager::~PreloadManager()
{
	for (auto& group : m_loadList)
	{
		if (group.second.thread.joinable()) group.second.thread.join();
	}
}

