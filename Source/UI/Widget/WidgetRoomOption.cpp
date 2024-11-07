#include <imgui.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneGame.h"
#include "UI/Widget/WidgetRoomOption.h"

#include "GameData.h"

#define DEFAULT_PORT 7000

WidgetRoomOption::WidgetRoomOption()
{
	if (GAME_DATA.GetUser().id < 0)
	{
		strcpy_s(name, 32, (char*)u8"プレイヤー");
	}
	else
	{
		strcpy_s(name, 32, GAME_DATA.GetUser().nickname.c_str());
	}
	strcpy_s(address, 32, GAME_DATA.GetIp().c_str());
	strcpy_s(port, 16, std::to_string(DEFAULT_PORT).c_str());
}

void WidgetRoomOption::Render(const RenderContext& rc)
{
	ImVec2 Center = ImGui::GetMainViewport()->Pos;
	Center.x += SCREEN_W * 0.5f - 150.0f;
	Center.y += SCREEN_H * 0.7f;
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		Center,
		ImGuiCond_Always
	);

	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);

	if (ImGui::Begin("Connection##Tile", nullptr,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoSavedSettings
	))
	{
		if (ImGui::BeginTable("Room", 2))
		{
			ImGui::TableSetupColumn("##text", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("##value", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text((char*)u8"名前");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1);
			if (GAME_DATA.GetUser().id < 0)
			{
				ImGui::InputText("##name", name, sizeof(name));
			}
			else
			{
				ImGui::InputText("##name", name, sizeof(name), ImGuiInputTextFlags_ReadOnly);
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text((char*)u8"IPアドレス");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputText("##ipaddress", address, sizeof(address));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text((char*)u8"ポート");
			ImGui::TableSetColumnIndex(1);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputText("##port", port, sizeof(port));

			ImGui::EndTable();
		}

		if (ImGui::Button((char*)u8"新しい部屋"))
		{
			if (GAME_DATA.GetUser().id < 0) NameWithToken();
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame(name, address, port, new HostNetworkController)));
		}
		ImGui::SameLine();
		if (ImGui::Button((char*)u8"入室"))
		{
			if (GAME_DATA.GetUser().id < 0) NameWithToken();
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame(name, address, port, new ClientNetworkController)));
		}
		ImGui::End();
	}
}

void WidgetRoomOption::NameWithToken()
{
	// ランダムプレイヤー名
	std::string token = name;
	token += (char*)u8"#";
	for (int i = 0; i < 4; i++)
	{
		token += std::to_string(RANDOMI(0, 9));
	}
	strcpy_s(name, 32, token.c_str());
}