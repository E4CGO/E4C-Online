//! @file Teleporter.h
//! @note 

#ifndef __INCLUDE_TELEPORTER_H__
#define __INCLUDE_TELEPORTER_H__

#include "GameObject/ModelObject.h"
#include "Scene/Stage/Stage.h"

/**************************************************************************//**
	@class	Teleporter
	@brief	ステージ転送用ゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class Teleporter : public ModelObject
{
public:
	Teleporter(Stage* stage);
	virtual ~Teleporter() = default;

	virtual void Update(float elapsedTime) override;
private:
	Stage* m_pStage;
	
	float m_portalTime = 3.0f;
	float m_timer = 0.0f;
	float m_radius = 10.0f;
};
#endif // !__INCLUDE_TELEPORTER_H__
