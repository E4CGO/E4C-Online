#pragma once

#include <memory>
#include <thread>

#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/FreeCameraController.h"
#include "TAKOEngine/Rendering/LightManager.h"
#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/Model/Model.h"
#include "TAKOEngine/Rendering/PostprocessingRenderer.h"
#include "TAKOEngine/Rendering/ShadowMapRender.h"

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	// 初期化
	virtual void Initialize() = 0;
	// 終了化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) {};
	// 描画処理
	virtual void Render() {};
	virtual void RenderDX12() {};

	// 準備確認
	bool IsReady() const { return ready; }
	// 準備完了
	void SetReady() { ready = true; }

protected:
	bool ready = false;
};

// モデルテストシーン
class ModelTestScene : public Scene
{
public:
	ModelTestScene() {};
	~ModelTestScene() override {};

	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override {};

	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
private:
	// シーンGUI描画
	void DrawSceneGUI();
	// プロパティGUI描画
	void DrawPropertyGUI();
	// アニメーションGUI描画
	void DrawAnimationGUI();

	std::unique_ptr<iModel> model;

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };

	iModel::Node* selectionNode = nullptr;

	FreeCameraController cameraController;

	bool animationLoop = false;
	float animationBlendSeconds = 0;

	std::unique_ptr<PostprocessingRenderer>	postprocessingRenderer; //	ポストプロセス
};

// ロードシーン
class SceneLoading : public Scene
{
public:
	SceneLoading(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoading() override {}
	// 初期化
	void Initialize() override;
	// 終了化
	void Finalize() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画処理
	void Render() override;
	void RenderDX12() override;
private:
	// ローディングスレッド
	static void LoadingThread(SceneLoading* scene);
private:
	Scene* nextScene = nullptr;
	std::thread* thread = nullptr;
};
