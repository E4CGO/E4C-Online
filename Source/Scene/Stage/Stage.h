//! @file Stage.h
//! @note 

#ifndef __INCLUDED_STAGE_H__
#define __INCLUDED_STAGE_H__

#include <DirectXMath.h>

struct ENEMY_LIST_DATA
{
	int phase = -1;
	float time = 0;
	int enemyType = -1;
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 rotation = {};
	
};

/**************************************************************************//**
	@class	Stage
	@brief	ステージ
	@par	[説明]
*//***************************************************************************/
class Stage
{
public:
	Stage() = default;
	virtual ~Stage() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update(float elapsedTime);

	virtual void DefeatBoss() {}

	virtual void Render() = 0;

	virtual void RenderDX12() = 0;

	bool IsReady() { return m_ready; }
	void SetReady() { m_ready = true; }
protected:
	float m_timer = 0.0f;
	bool m_ready = false;;

};

#endif // !__INCLUDED_STAGE_H__