#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include "TAKOEngine/Rendering/Command.h"
#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/SpriteDX12.h"

// リソースマネージャー
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}

public:
	// 唯一のインスタンス取得
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// 更新処理
	void Run();

	// クリア
	void Clear();

	// ガベージコレクション
	void GarbageCollect();

	// 読み込み中のリソースが読み込み完了するまで待つ
	void WaitOnPending();

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	std::shared_ptr<ModelResource> LoadModelDX12Resource(const char* filename);
	std::shared_ptr<ModelResource> LoadModelResourceGLTF(std::string filename);
	std::shared_ptr<ModelResource> LoadImmediateDX12Resource(const char* filename);
	std::shared_ptr<ModelResource> LoadAsyncDX12Resource(const char* filename);
	std::shared_ptr<Sprite> LoadSpriteResource(const char* filename);
	std::shared_ptr<SpriteDX12> LoadSpriteResourceDX12(const char* filename);

private:
	using ModelMap = std::unordered_map<std::string, std::weak_ptr<ModelResource>>;
	ModelMap models;
	using SpriteMap = std::unordered_map<std::string, std::weak_ptr<Sprite>>;
	using SpriteMapDX12 = std::unordered_map<std::string, std::weak_ptr<SpriteDX12>>;
	SpriteMap sprites;
	SpriteMapDX12 spritesDX12;

	// 計測
	void BeginTimer() { m_loading_count = m_loaded_count = 0; }
	void EndTimer() { m_load_count = m_loading_count; }

	int									m_load_count = 0;
	int									m_loading_count = 0;
	int									m_loaded_count = 0;

	std::mutex m_mut;

	struct LoadingInfo
	{
		std::string						filename;
		std::shared_ptr<ModelResource>	resource;
	};

	struct LoadingThread
	{
		ResourceManager* owner = nullptr;
		std::unique_ptr<std::thread>	thread;
		HANDLE							wakeup_handle = NULL;
		HANDLE							sleep_handle = NULL;
		volatile bool					exit = false;

		std::mutex						mutex;
		std::vector<LoadingInfo>		loading_infos;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	d3d_copy_command_queue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>	d3d_direct_command_queue;
		Microsoft::WRL::ComPtr<ID3D12Fence>			d3d_fence;
		std::unique_ptr<CommandList>				copy_command_list;
		std::unique_ptr<CommandList>				direct_command_list;

		HANDLE										fence_event = NULL;
		UINT64										fence_value = 0;

		void Run();
	};

	std::unique_ptr<std::thread>		m_thread;
	std::mutex							m_mutex;
	HANDLE								m_wakeup_handle = NULL;
	volatile bool						m_exit = false;

	static const int LoadingThreadNum = 12;
	int									m_use_thread_num = LoadingThreadNum;

	LoadingThread						m_loading_threads[LoadingThreadNum];

	ModelMap							m_pendings;
	ModelMap							m_loadeds;
};

#define RESOURCE ResourceManager::Instance()