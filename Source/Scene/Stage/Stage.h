#pragma once

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

/*
* ステージ管理用・ホスト
*/
class Stage
{
public:
	Stage() = default;
	~Stage() {};

	virtual void Initialize();

	virtual void Update(float elapsedTime);

	void SetPhase(int phase);
	void NextPhase();
	int GetPhase() { return phase; }

	void Finish() { finish = true; }
	bool IsFinish() { return finish; }
protected:
	virtual void OnPhase() {}
protected:
	int phase;
	float timer = 0.0f;
	bool finish = false;

	std::vector<ENEMY_LIST_DATA> enemyList;
	int pointer = 0;
};
