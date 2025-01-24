#include "WidgetLogin.h"

#include "TAKOEngine/Network/WinSock2Wrapper.h"
#include <shellapi.h>
#include <imgui.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Network/url.h"
#include "TAKOEngine/Tool/Encode.h"

#include "GameData.h"

WidgetLogin::WidgetLogin()
{
	memset(username, 0, sizeof(username));
	memset(password, 0, sizeof(password));
}

void WidgetLogin::Update(float elapsedTime)
{
	if (xhr != nullptr)
	{
		if (xhr->IsDone())
		{
			HttpResponse response = xhr->GetResponse();
			if (response.status == 200) // 成功
			{
				std::string token = response.data["token"];
				memset(GAME_DATA.GetSetting().token, 0, sizeof(GAME_DATA.GetSetting().token));
				token.copy(GAME_DATA.GetSetting().token, token.size());
				GAME_DATA.SaveGameSetting();

				state = STATE::SUCCESS;
			}
			else
			{
				// 200以外: 失敗
				state = STATE::FAIL;
			}
			delete xhr;
			xhr = nullptr;
		}
	}
}

void WidgetLogin::Render(const RenderContext& rc)
{
	switch (state)
	{
	case WidgetLogin::INPUT:
	case WidgetLogin::FAIL:
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

		if (ImGui::Begin("Window##Login", nullptr,
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoSavedSettings
		))
		{
			if (ImGui::BeginTable("Login", 2))
			{
				ImGui::TableSetupColumn("##text", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("##value", ImGuiTableColumnFlags_WidthStretch);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text((char*)u8"ユーザー");
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##username", username, sizeof(username));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text((char*)u8"パスワード");
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGui::InputText("##password", password, sizeof(password), ImGuiInputTextFlags_Password);

				if (state)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(-1);
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
					ImGui::Text((char*)u8"ログイン失敗");
					ImGui::PopStyleColor();
				}

				ImGui::EndTable();
			}

			if (ImGui::Button((char*)u8"ログイン"))
			{
				xhr = new HttpRequest(URL(Url::LOGIN));
				xhr->SetMethod(HttpRequest::METHOD::POST);
				xhr->SetData({
					{ "username", username },
					{ "password", password }
					});
				xhr->Send();
				state = STATE::LOADING;
			}
			ImGui::SameLine();
			if (ImGui::Button((char*)u8"サインアップ"))
			{
				ShellExecute(0, 0, Encode::string_to_wstring(URL(Url::REGISTER_PAGE)).c_str(), 0, 0, SW_SHOW);
			}
			ImGui::End();
		}
		break;
	}
	case WidgetLogin::LOADING:
	case WidgetLogin::SUCCESS:
		break;
	}
}

void WidgetLogin::RenderDX12(const RenderContextDX12& rc)
{
}