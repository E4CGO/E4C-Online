//! @file PreloadManager.h
//! @note 

#ifndef __INCLUDE_PRELOAD_MANAGER_H__
#define __INCLUDE_PRELOAD_MANAGER_H__

#include <thread>
#include <string>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Tool/Singleton.h"

/**************************************************************************//**
	@class	PreloadManager
	@brief	プリロード管理クラス
	@par	[説明]
		モデル、画像のロードをマルチスレッド管理用クラス
*//***************************************************************************/
class PreloadManager : public Singleton<PreloadManager>
{
	friend class Singleton <PreloadManager>;
private:
	PreloadManager();
	~PreloadManager();

	enum RESORUCE_TYPE : uint8_t
	{
		SPRITE,
		MODEL,
	};
	struct LOAD_GROUP
	{
		RESORUCE_TYPE type;
		std::unordered_set<std::string> list = {};
		std::thread thread;
	};
private:
	void LoadThread(const std::string& group);
public:
	void NewGroup(const std::string& group, const RESORUCE_TYPE& type)
	{
		if (!m_loadList.contains(group))
		{
			m_loadList[group] = LOAD_GROUP{
				type
			};
			m_groupOrder.push_back(group);
		}
	}
	void RegisterFile(const std::string& group, const std::string& filename)
	{
		if (!m_loadList.contains(group))
		{
			m_loadList[group].list.insert(filename);
		}
	}
	void Load();

	void Join(std::string group)
	{
		if (m_loadList.contains(group) && m_loadList[group].thread.joinable())
		{
			m_loadList[group].thread.join();
		}
	}
private:
	std::unordered_map<std::string, LOAD_GROUP> m_loadList;
	std::vector<std::string> m_groupOrder;

	std::unordered_set<std::shared_ptr<SpriteDX12>> m_spriteD12List;
	std::unordered_set<std::shared_ptr<ModelResource>> m_modelList;
	
	std::mutex m_mtLoading;
};

#define PRELOAD PreloadManager::Instance()
#endif // !__INCLUDE_PRELOAD_MANAGER_H__
