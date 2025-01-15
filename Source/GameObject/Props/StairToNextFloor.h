//! @file StairToNextFloor.h
//! @note

#ifndef __INCLUDE_STAIRTONEXTFLOOR__
#define __INCLUDE_STAIRTONEXTFLOOR__

#include "GameObject/Props/Teleporter.h"
#include "Scene/Stage/Stage.h"
#include "UI/Widget/WidgetMatching.h"
#include "Network/OnlineController.h"

/**************************************************************************//**
	@class	StairToNextFloor
	@brief	次の階へ転送するためのゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class StairToNextFloor : public Teleporter
{
public:
	StairToNextFloor() : Teleporter(nullptr, nullptr) {}
	virtual void Update(float elapsedTime) override;
};

#endif // !__INCLUDE_STAIRTONEXTFLOOR__