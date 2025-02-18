#include "Graphics.h"
#include "ResourceManager.h"

void ResourceManager::Run()
{
	while (!m_exit)
	{
		// スレッドが起きるまで待つ
		{
			WaitForSingleObject(m_wakeup_handle, INFINITE);
		}
		if (m_exit)
		{
			// ローディングスレッドを終了する
			for (int i = 0; i < LoadingThreadNum; ++i)
			{
				LoadingThread& loading_thread = m_loading_threads[i];
				loading_thread.exit = true;
				SetEvent(loading_thread.wakeup_handle);
				loading_thread.thread->join();
			}
			break;
		}
		if (m_loaded_count == 0)
		{
			//T_TIMER.Tick();
		}

		bool pending = true;
		while (pending)
		{
			// ペンディング情報をローディング情報へ受け渡し
			int use_thread_num = 0;
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto it = m_pendings.begin();
				if (it == m_pendings.end())
				{
					pending = false;
					continue;
				}

				int index = 0;
				while (it != m_pendings.end())
				{
					LoadingThread& loading_thread = m_loading_threads[index];
					{
						std::lock_guard<std::mutex> lock(loading_thread.mutex);

						LoadingInfo& loading_info = loading_thread.loading_infos.emplace_back();
						loading_info.filename = it->first.c_str();
						loading_info.resource = it->second.lock();
					}

					index = (index + 1) % LoadingThreadNum;
					use_thread_num++;
#if 0
					if (use_thread_num > 20 * LoadingThreadNum)
					{
						break;
					}
#endif
					it++;
				}
				if (use_thread_num > LoadingThreadNum)
				{
					use_thread_num = LoadingThreadNum;
				}
			}

			//--------------------------
			// NOTE:③リソースマネージャー⑨
			//--------------------------
			// ローディングスレッドを起動
			HANDLE loading_thread_handles[LoadingThreadNum];
			{
				for (int i = 0; i < use_thread_num; ++i)
				{
					LoadingThread& loading_thread = m_loading_threads[i];

					SetEvent(loading_thread.wakeup_handle);
					loading_thread_handles[i] = loading_thread.sleep_handle;
				}
			}

			//--------------------------
			// NOTE:③リソースマネージャー⑫
			//--------------------------
			// サブスレッドのリソース読み込み処理完了まで待つ
			{
				WaitForMultipleObjects(use_thread_num, loading_thread_handles, true, INFINITE);
			}

			// キャッシュに追加し、ペンディングを削除する
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				for (int i = 0; i < use_thread_num; ++i)
				{
					LoadingThread& loading_thread = m_loading_threads[i];
					for (LoadingInfo& loading_info : loading_thread.loading_infos)
					{
						m_loadeds[loading_info.filename] = loading_info.resource;
						m_pendings.erase(loading_info.filename);
					}
					loading_thread.loading_infos.clear();
				}
			}

			if (m_exit)
			{
				// ローディングスレッドを終了する
				for (int i = 0; i < LoadingThreadNum; ++i)
				{
					LoadingThread& loading_thread = m_loading_threads[i];
					loading_thread.exit = true;
					SetEvent(loading_thread.wakeup_handle);
					loading_thread.thread->join();
				}
				break;
			}
		}
		if (m_loaded_count == m_load_count)
		{
			//T_TIMER.Tick();
		}
	}
}

void ResourceManager::Clear()
{
	WaitOnPending();

	m_pendings.clear();
	m_loadeds.clear();
}

void ResourceManager::GarbageCollect()
{
	WaitOnPending();

	std::lock_guard<std::mutex> lock(m_mutex);

	// モデル
	{
		bool found = true;
		while (found)
		{
			found = false;
			auto it = m_loadeds.begin();
			while (it != m_loadeds.end())
			{
				if (it->second.expired())
				{
					found = true;
					//LOG("Unloading %s\n", it->first.c_str());
					auto temp = it;
					++it;
					m_loadeds.erase(temp);
				}
				else
				{
					++it;
				}
			}
		}
	}
	for (auto loaded : m_loadeds)
	{
		//	LOG("Loaded %s\n", loaded.first.c_str());
	}
}

void ResourceManager::WaitOnPending()
{
	size_t numPending = 0;
	do
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			numPending = m_pendings.size();
		}

		if (numPending)
		{
			Sleep(16);
		}
	} while (numPending);
}

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
	if (strlen(filename) == 0) return nullptr;

	std::lock_guard<std::mutex> lock(m_mut);
	// モデル検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みモデルのリソースを返す
			return it->second.lock();
		}
	}

	// 新規モデルリソース作成&読み込み
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	model->Load(Graphics::Instance().GetDevice(), filename);

	// マップに登録
	models[filename] = model;

	return model;
}

std::shared_ptr<ModelResource> ResourceManager::LoadModelDX12Resource(const char* filename)
{
	if (strlen(filename) == 0) return nullptr;

	std::lock_guard<std::mutex> lock(m_mut);
	// モデル検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みモデルのリソースを返す
			return it->second.lock();
		}
	}

	// 新規モデルリソース作成&読み込み
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	model->Load(filename);

	// マップに登録
	models[filename] = model;

	return model;
}

std::shared_ptr<ModelResource> ResourceManager::LoadModelResourceGLTF(std::string filename)
{
	if (filename.size() == 0) return nullptr;

	std::lock_guard<std::mutex> lock(m_mut);
	// モデル検索
	ModelMap::iterator it = models.find(filename);
	if (it != models.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みモデルのリソースを返す
			return it->second.lock();
		}
	}

	// 新規モデルリソース作成&読み込み
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	model->LoadGLTF(Graphics::Instance().GetDevice(), filename);

	// マップに登録
	models[filename] = model;

	return model;
}

std::shared_ptr<ModelResource> ResourceManager::LoadImmediateDX12Resource(const char* filename)
{
	// リソースを検索
	ModelMap::iterator it = m_loadeds.find(filename);
	if (it != m_loadeds.end())
	{
		// リンク（寿命）が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みのモデルリソースを返す
			return it->second.lock();
		}
	}

	// 新規リソース作成＆読み込み
	std::shared_ptr<ModelResource> model = std::make_shared<ModelResource>();
	model->Load(filename);

	// マップに登録
	m_loadeds[filename] = model;

	return model;
}

std::shared_ptr<ModelResource> ResourceManager::LoadAsyncDX12Resource(const char* filename)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::shared_ptr<ModelResource> model;

	// 読み込み済みリソースがあるかチェック
	{
		ModelMap::iterator it = m_loadeds.find(filename);
		if (it != m_loadeds.end())
		{
			// リンク（寿命）が切れていないか確認
			if (!it->second.expired())
			{
				model = it->second.lock();
			}
		}
	}

	// 読み込み中リソースがあるかチェック
	if (model == nullptr)
	{
		ModelMap::iterator it = m_pendings.find(filename);
		if (it != m_pendings.end())
		{
			model = it->second.lock();
		}
	}

	// 新規リソースを作成し、読み込み中リストに追加
	if (model == nullptr)
	{
		model = std::make_shared<ModelResource>();
		m_pendings[filename] = model;
		// スレッドを起こす
		SetEvent(m_wakeup_handle);

		m_loading_count++;
	}

	return model;
}

std::shared_ptr<Sprite> ResourceManager::LoadSpriteResource(const char* filename)
{
	std::lock_guard<std::mutex> lock(m_mut);
	// スプライト検索
	SpriteMap::iterator it = sprites.find(filename);
	if (it != sprites.end())
	{
		// リンク(寿命)が切れていないか確認
		if (!it->second.expired())
		{
			// 読み込み済みスプライトのリソースを返す
			return it->second.lock();
		}
	}

	// 新規スプライトリソース作成&読み込み
	std::shared_ptr<Sprite> sprite;
	if (std::string(filename).length() == 0)
	{
		sprite = std::make_shared<Sprite>(Graphics::Instance().GetDevice());
	}
	else
	{
		sprite = std::make_shared<Sprite>(Graphics::Instance().GetDevice(), filename);
	}

	// マップに登録
	sprites[filename] = sprite;

	return sprite;
}

std::shared_ptr<SpriteDX12> ResourceManager::LoadSpriteResourceDX12(const char* filename)
{
	std::lock_guard<std::mutex> lock(m_mut);
	//スプライト検索
	//SpriteMapDX12::iterator it = spritesDX12.find(filename);
	//if (it != spritesDX12.end())
	//{
	//	// リンク(寿命)が切れていないか確認
	//	if (!it->second.expired())
	//	{
	//		// 読み込み済みスプライトのリソースを返す
	//		return it->second.lock();
	//	}
	//}

	// 新規スプライトリソース作成&読み込み
	std::shared_ptr<SpriteDX12> sprite;
	if (std::string(filename).length() == 0)
	{
		sprite = std::make_shared<SpriteDX12>(1);
	}
	else
	{
		sprite = std::make_shared<SpriteDX12>(1, filename);
	}

	// マップに登録
	spritesDX12[filename] = sprite;

	return sprite;
}

void ResourceManager::LoadingThread::Run()
{
	CoInitialize(nullptr);

	//--------------------------
	// NOTE:③リソースマネージャー⑩
	//--------------------------
	while (!exit)
	{
		// スレッドが起きるまで待つ
		{
			WaitForSingleObject(wakeup_handle, INFINITE);
		}
		if (exit)
		{
			::SetEvent(sleep_handle);
			break;
		}

		// ローディング処理
		{
			std::lock_guard<std::mutex> lock(mutex);

			ID3D12GraphicsCommandList* d3d_copy_command_list = copy_command_list->GetCommandList();
			ID3D12GraphicsCommandList* d3d_direct_command_list = direct_command_list->GetCommandList();
			std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> d3d_upload_resources;

			// コマンド積み込み開始
			{
				copy_command_list->Begin();
				direct_command_list->Begin();
			}

			// リソース読み込み処理
			for (LoadingInfo& loading_info : loading_infos)
			{
				// TODO
				loading_info.resource->Load(
					loading_info.filename.c_str());
			}

			// コマンド積み込み終了
			{
				copy_command_list->End();
				direct_command_list->End();
			}

			// コピーコマンド実行
			{
				ID3D12CommandList* d3d_command_lists[] =
				{
					d3d_copy_command_list,
				};
				d3d_copy_command_queue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);
			}

			// コピーコマンド処理終了待ち設定
			{
				d3d_copy_command_queue->Signal(d3d_fence.Get(), ++fence_value);
				d3d_direct_command_queue->Wait(d3d_fence.Get(), fence_value);
			}

			// バリアコマンド実行
			{
				ID3D12CommandList* d3d_command_lists[] =
				{
					d3d_direct_command_list,
				};
				d3d_direct_command_queue->ExecuteCommandLists(_countof(d3d_command_lists), d3d_command_lists);
			}

			// コマンド処理終了待ち
			{
				d3d_direct_command_queue->Signal(d3d_fence.Get(), ++fence_value);
				if (d3d_fence->GetCompletedValue() < fence_value)
				{
					// コマンド完了時にイベントが発生するように設定
					d3d_fence->SetEventOnCompletion(fence_value, fence_event);

					// イベントが発生するまで待つ
					WaitForSingleObject(fence_event, INFINITE);
				}
			}

			//--------------------------
			// NOTE:③リソースマネージャー⑪
			//--------------------------
			// リソース準備完了設定
			for (LoadingInfo& loading_info : loading_infos)
			{
				// TODO
				loading_info.resource->SetReady();
			}
		}

		// 処理完了通知
		{
			::SetEvent(sleep_handle);
		}
	}

	// COMライブラリの終了処理
	CoUninitialize();
}