#pragma once
#include "Enemy.h"
class Chest : public Enemy
{
public:
	Chest(const char* filename);

	void OnDeath() override;

	void UpdateColliders() override;
protected:
	enum CHEST_STATE
	{
		CLOSE,
		OPEN
	};
};

class EmptyChest : public Chest
{
public:
	EmptyChest();
	EmptyChest(std::function<void(Enemy*)> OnOpen);

	void SetOnOpen(std::function<void(Enemy*)> OnOpen);
};

class VictoryChest : public Chest
{
public:
	VictoryChest();
protected:
	void OnOpen();
};