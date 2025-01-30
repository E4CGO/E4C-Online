//! @file OneWayWall.h
//! @note

#ifndef __ONE_WAY_WALL_H__
#define __ONE_WAY_WALL_H__

#include "GameObject/ModelObject.h"

/**************************************************************************//**
	@class	OneWayWall
	@brief	一歩通行壁クラス
	@par	[説明]
*//***************************************************************************/
class OneWayWall : public ModelObject
{
public:
	enum WayDirection
	{
		MinusZ = 0,
		MinusX,
		PlusZ,
		PlusX,
	};

	OneWayWall(WayDirection dir, float scale = 3.0f, const char* filename = "Data/Model/Object/Plane.glb");
	virtual ~OneWayWall() = default;

	void Update(float elapsedTime) override;

	void CollisionTrigger(Collider* myCol, Collider* otherCol);
	void CollisionWall(Collider* myCol, Collider* otherCol);
private:
	float m_Scaling = 3.0f;
};

#endif // !__ONE_WAY_WALL_H__
