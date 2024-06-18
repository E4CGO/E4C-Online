#pragma once

#include "Scene.h"

class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}
public:
	// 唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
	// 更新処理
	void Update(float elapsedTime);
	// 描画処理
	void Render();
	void RenderDX12();
	// シーンクリア
	void Clear();
	// シーン切り替え
	void ChangeScene(Scene* scene);
private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};