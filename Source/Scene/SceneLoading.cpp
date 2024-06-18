#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

// 初期化
void SceneLoading::Initialize()
{
	thread = new std::thread(LoadingThread, this);
}
// 終了化
void SceneLoading::Finalize()
{
	if (thread != nullptr)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	}
}
// 更新処理
void SceneLoading::Update(float elapsedTime)
{
	if (nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}
// 描画処理
void SceneLoading::Render()
{
	// 画面クリア
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0, 0, 0, 1);

	T_TEXT.Begin();

	T_TEXT.Render(
		FONT_ID::HGpop,
		"Now Loading...",
		T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f,
		FONT_ALIGN::BOTTOM_RIGHT
	);

	T_TEXT.End();
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	srand(static_cast <unsigned> (time(NULL)));

	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	// 次のシーンの初期化を行う
	scene->nextScene->Initialize();

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
}