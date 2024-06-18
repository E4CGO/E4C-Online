#include <imgui.h>
#include <functional>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/ModelResource.h"
#include "TAKOEngine/Tool/TransformUtils.h"

#include "Scene/Scene.h"

// 初期化
void ModelTestScene::Initialize()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	float screenWidth = Graphics::Instance().GetScreenWidth();
	float screenHeight = Graphics::Instance().GetScreenHeight();

	// カメラ設定
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 画角
		screenWidth / screenHeight,			// 画面アスペクト比
		0.1f,								// ニアクリップ
		10000.0f							// ファークリップ
	);
	camera.SetLookAt(
		{ 0, 8, 15 },	// 視点
		{ 0, 2, 0 },	// 注視点
		{ 0, 1, 0 }		// 上ベクトル
	);

	// モデル作成
	//model = std::make_unique<Model>(device, "Data/Model/Slime/Slime.fbx", 1.0f);
	//model = std::make_unique<Model>(device, "Data/Model/Mari.O/Mari-O.fbx", 1.0f);
	model = std::make_unique<Model>(device, "Data/Model/Cube/testCubes.glb", 1.0f);
	//model->PlayAnimation(1, true);
	//model = std::make_unique<Model>(device, "Data/Model/MariLuiHouse/MariLuiHouse.fbx", 1.0f);
	//model->SetLinearGamma(2.2f);

	cameraController.SyncCameraToController(camera);

	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// 点光源を追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(0, 0, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		LightManager::Instance().Register(light);
	}

	// ポストプロセス描画クラス生成
	{
		postprocessingRenderer = std::make_unique<PostprocessingRenderer>();
		//postprocessingRenderer->SetBloom
		//(
		//	{
		//		{0.23f, 0.813f},
		//		{2, 10.0f}
		//	}
		//);
		//postprocessingRenderer->SetColoGrading
		//(
		//	{
		//		0,
		//		2.0f,
		//		0.813f
		//	}
		//);
	}
}

// 更新処理
void ModelTestScene::Update(float elapsedTime)
{
	// カメラ処理更新
	cameraController.Update();
	cameraController.SyncContrllerToCamera(camera);

	// ワールド行列計算
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMFLOAT4X4 worldTransform;
	DirectX::XMStoreFloat4x4(&worldTransform, S * R * T);

	// アニメーション更新
	model->UpdateAnimation(elapsedTime);

	// トランスフォーム更新
	model->UpdateTransform(worldTransform);
}

// 描画処理
void ModelTestScene::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->Clear(T_GRAPHICS.GetDeviceContext(), 0.5f, 0.5f, 0.5f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->SetRenderTargets(T_GRAPHICS.GetDeviceContext());

	// 描画コンテキスト設定
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ライトの情報を詰め込む
	LightManager::Instance().PushRenderContext(rc);

	// 描画
	ModelShader* shader = T_GRAPHICS.GetModelShader(ModelShaderId::Phong);
	shader->Begin(rc);
	shader->Draw(rc, model.get());
	shader->End(rc);

	// ライトのデバッグプリミティブの描画
	LightManager::Instance().DrawDebugPrimitive();
	postprocessingRenderer->DrawDebugGUI();

	// ライトのデバッグプリミティブの描画
	LightManager::Instance().DrawDebugGUI();
	// ラインレンダラ描画実行
	T_GRAPHICS.GetLineRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());
	// デバッグレンダラ描画実行
	T_GRAPHICS.GetDebugRenderer()->Render(T_GRAPHICS.GetDeviceContext(), camera.GetView(), camera.GetProjection());

	// 書き込み先をバックバッファに変えてオフスクリーンレンダリングの結果を描画する
	{
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.5f, 1.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTargets(T_GRAPHICS.GetDeviceContext());

		//	ポストプロセス処理を行う
		postprocessingRenderer->Render(T_GRAPHICS.GetDeviceContext());
	}
	T_TEXT.Begin();

	T_TEXT.TextBox(
		FONT_ID::HGpop,
		std::string("マリー・〇＆ルイー・Ｇ　ＲＰＧ　マリー・〇＆ルイー・Ｇ　ＲＰＧ　マリー・〇＆ルイー・Ｇ　ＲＰＧ　マリー・〇＆ルイー・Ｇ　ＲＰＧ　マリー・〇＆ルイー・Ｇ　ＲＰＧ　マリー・〇＆ルイー・Ｇ　ＲＰＧ"),
		10,
		0, 0,
		1, 1, 1, 1,
		0.7f
	);
	T_TEXT.End();

	// デバッグメニュー描画
	DrawSceneGUI();
	DrawPropertyGUI();
	DrawAnimationGUI();
}

// シーンGUI描画
void ModelTestScene::DrawSceneGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 10, pos.y + 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_None))
	{
		DirectX::XMFLOAT3 eye = camera.GetEye();
		ImGui::DragFloat3("Eye", &eye.x, 0.01f, 100.0f);

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.1f);

			// 回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::DragFloat3("Angle", &a.x, 1.0f);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);

			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);
		}

		if (ImGui::CollapsingHeader("Hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// ノードツリーを再帰的に描画する関数
			std::function<void(Model::Node*)> drawNodeTree = [&](Model::Node* node)
				{
					// 矢印をクリック、またはノードをダブルクリックで階層を開く
					ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

					// 子がいない場合は矢印をつけない
					size_t childCount = node->children.size();
					if (childCount == 0)
					{
						nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					}

					// 選択フラグ
					if (selectionNode == node)
					{
						nodeFlags |= ImGuiTreeNodeFlags_Selected;
					}

					// ツリーノードを表示
					bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name.c_str());

					// フォーカスされたノードを選択する
					if (ImGui::IsItemFocused())
					{
						selectionNode = node;
					}

					// 開かれている場合、子階層も同じ勝利を行う
					if (opened && childCount > 0)
					{
						for (Model::Node* child : node->children)
						{
							drawNodeTree(child);
						}
						ImGui::TreePop();
					}
				};
			// 再帰的にノードを描画
			drawNodeTree(model->GetRootNode());
		}

		ImGui::End();
	}
}

// プロパティGUI描画
void ModelTestScene::DrawPropertyGUI()
{
	ImVec2 pos = ImGui::GetMainViewport()->Pos;
	ImGui::SetNextWindowPos(ImVec2(pos.x + 970, pos.y + 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("Property", nullptr, ImGuiWindowFlags_None);

	if (selectionNode != nullptr)
	{
		if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::DragFloat3("Position", &selectionNode->position.x, 0.1f);

			// 回転
			DirectX::XMFLOAT3 angle;
			TransformUtils::QuaternionToRollPitchYaw(selectionNode->rotation, angle.x, angle.y, angle.z);
			angle.x = DirectX::XMConvertToDegrees(angle.x);
			angle.y = DirectX::XMConvertToDegrees(angle.y);
			angle.z = DirectX::XMConvertToDegrees(angle.z);
			if (ImGui::DragFloat3("Rotation", &angle.x, 1.0f))
			{
				angle.x = DirectX::XMConvertToRadians(angle.x);
				angle.y = DirectX::XMConvertToRadians(angle.y);
				angle.z = DirectX::XMConvertToRadians(angle.z);
				DirectX::XMVECTOR Rotation = DirectX::XMQuaternionRotationRollPitchYaw(angle.x, angle.y, angle.z);
				DirectX::XMStoreFloat4(&selectionNode->rotation, Rotation);
			}
			// スケール
			ImGui::DragFloat3("Scale", &selectionNode->scale.x, 0.01f);
		}
	}
	ImGui::End();
}

// アニメーションGUI描画
void ModelTestScene::DrawAnimationGUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 350), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("Animation", nullptr, ImGuiWindowFlags_None);

	ImGui::Checkbox("Loop", &animationLoop);

	int index = 0;
	ImGui::DragFloat("BlendSec", &animationBlendSeconds, 0.01f);

	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Animation& animation : resource->GetAnimations())
	{
		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

		ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str());

		// ダブルクリックでアニメーション再生
		if (ImGui::IsItemClicked())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				model->PlayAnimation(index, animationLoop, animationBlendSeconds);
			}
		}
		ImGui::TreePop();
		index++;
	}

	ImGui::End();
}