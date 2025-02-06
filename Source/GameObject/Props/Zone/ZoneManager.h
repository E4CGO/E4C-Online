//! @file ZoneManager.h
//! @note 

#ifndef __INCLUDE_ZONE_MANAGER_H__
#define __INCLUDE_ZONE_MANAGER_H__

#include "TAKOEngine/Tool/Singleton.h"
#include "GameObject/ObjectManager.h"
#include "GameObject/Props/Zone/ZoneObject.h"

/**************************************************************************//**
	@class	ZoneManager
	@brief	ゾーン管理クラス
	@par	[説明]
		生成したゾーンを管理するクラス
*//***************************************************************************/
class ZoneManager : public ObjectManager<ZoneObject>, public Singleton<ZoneManager>
{
	friend class Singleton<ZoneManager>;
private:
	// コンストラクタ
	ZoneManager() = default;
	// デストラクタ
	~ZoneManager() = default;
};

#endif // !__INCLUDE_ZONE_MANAGER_H__
