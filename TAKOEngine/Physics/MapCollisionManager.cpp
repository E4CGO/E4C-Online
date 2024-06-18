#include "MapCollisionManager.h"
#include "UnrotatedBoxCollider.h"
#include "imgui.h"

MapCollisionManager::~MapCollisionManager()
{
	Clear();
}

void MapCollisionManager::DrawDebugGUI()
{
	if (ImGui::TreeNode("MapColliders"))
	{
		if (ImGui::Button("+")) {
			Register(new UnrotatedBoxCollider);
		}

		int nodeId = 0;
		for (Collider* collider : items)
		{
			ImGui::PushID(nodeId++);
			collider->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
}

void MapCollisionManager::DrawDebugPrimitive()
{
#ifdef _DEBUG
	for (Collider* collider : items)
	{
		collider->DrawDebugPrimitive();
	}
#endif // _DEBUG
}