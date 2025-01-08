//! @file Stage.h
//! @note 

#ifndef __INCLUDED_STAGE_H__
#define __INCLUDED_STAGE_H__

#include <DirectXMath.h>
#include <vector>



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

	void Spawne();

	virtual void Render() = 0;

	virtual void RenderDX12() = 0;

	void SetPhase(int phase);
	void NextPhase();
	int GetPhase() { return phase; }

	void Finish() { finish = true; }
	bool IsFinish() { return finish; }

	bool IsReady() { return m_ready; }
	void SetReady() { m_ready = true; }
protected:
	virtual void OnPhase() {}
protected:
	int phase;
	float m_timer = 0.0f;
	bool finish = false;

	std::vector<ENEMY_LIST_DATA> enemyList;
	int pointer = 0;

	bool m_ready = false;;

};

#endif // !__INCLUDED_STAGE_H__