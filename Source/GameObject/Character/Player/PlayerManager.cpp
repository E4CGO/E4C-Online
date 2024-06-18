#include "PlayerManager.h"

// デバッグ情報の表示
void PlayerManager::DrawDebugGUI()
{
}

// デバッグプリミティブの表示
void PlayerManager::DrawDebugPrimitive()
{
}

void PlayerManager::InitializePosition()
{
	int i = 0;
	int j = 1;
	float z = 3.0f;
	for (Player*& player : items)
	{
		DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, z };
		pos.x = 1.5f * ceil(i / 2.0f) * j;

		player->SetPosition(pos);
		i++;
		j *= -1;

		if (i > 6) {
			i = 0;
			z -= 1.5f;
		}
	}
}

Player* PlayerManager::GetPlayerById(int id)
{
	if (id < 0) return nullptr;
	for (Player*& player : items)
	{
		if (player->GetClientId() == id) return player;
	}

	return nullptr;
}