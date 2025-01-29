//! @file StairToNextFloor.h
//! @note

#ifndef __INCLUDE_STAIRTONEXTFLOOR__
#define __INCLUDE_STAIRTONEXTFLOOR__

#include "GameObject/Props/Teleporter.h"
#include "Scene/Stage/Stage.h"
#include "UI/Widget/WidgetMatching.h"
#include "Network/OnlineController.h"
#include "Scene/GameLoop/SceneGame/Stage/StageDungeon_E4C.h"
#include "TAKOEngine/Rendering/DebugRenderer/CylinderRenderer.h"

/**************************************************************************//**
	@class	StairToNextFloor
	@brief	次の階へ転送するためのゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class StairToNextFloor : public Teleporter
{
public:
	StairToNextFloor(Stage* stage, Online::OnlineController* onlineController);

public:
	virtual void Update(float elapsedTime) override;
	void RenderDX12(const RenderContextDX12& rc) override;
private:
	void GoToNextFloor();

	std::unique_ptr<CylinderRenderer> m_cylinderRenderer;

	bool enable = true;
};

#endif // !__INCLUDE_STAIRTONEXTFLOOR__