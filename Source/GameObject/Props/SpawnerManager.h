//! @file SpawnerManager.h
//! @note 

#ifndef __INCLUDE_SPAWNER_MANAGER_H__
#define __INCLUDE_SPAWNER_MANAGER_H__

#include "TAKOEngine/Tool/Singleton.h"
#include "GameObject/ObjectManager.h"
#include "GameObject/Props/Spawner.h"

/**************************************************************************//**
	@class	SpawnerManager
	@brief	スポナー管理用クラス
	@par	[説明]
		スポナーの管理用マネージャー
*//***************************************************************************/
class SpawnerManager : public ObjectManager<Spawner>, public Singleton<SpawnerManager>
{
	friend class Singleton<SpawnerManager>;
protected:
	SpawnerManager() = default;
	~SpawnerManager() = default;
public:
	// スポナーのアドレスを取得
	Spawner* GetByIndex(uint8_t index) {
		if (index >= items.size()) return nullptr;
		return items[index];
	};
	// スポナーのインデックスを取得
	const uint8_t GetIndex(Spawner* spawner);
};
#endif // !__INCLUDE_SPAWNER_MANAGER_H__