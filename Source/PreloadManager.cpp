//! @file PreloadManager.cpp
//! @note
#include "PreloadManager.h"

#include <fstream>
#include "External/tinygltf/json.hpp"

/**************************************************************************//**
	@brief	コンストラクタ
*//***************************************************************************/
PreloadManager::PreloadManager()
{
	{
		using json = nlohmann::json;
		std::ifstream f("Data/Model/preload.json");
		json data = json::parse(f);

		for (auto& group : data) {
			NewGroup(group["group"], group["type"]);
			const auto& list = group["list"];
			for (const auto& filename : list)
			{
				m_loadList[group["group"]].list.insert(filename);
			}
		}
		return;
	}
}

/**************************************************************************//**
	@brief	ロードスレッド生成
*//***************************************************************************/
void PreloadManager::Load()
{
	for (const std::string& group : m_groupOrder)
	{
		m_loadList[group].thread = std::thread(&PreloadManager::LoadThread, this, group);
	}
}
/**************************************************************************//**
	@brief		ロードスレッド関数
	@param[in]	group ロードグループネーム
*//***************************************************************************/
void PreloadManager::LoadThread(const std::string& group)
{
	if (m_loadList.contains(group))
	{
		m_mtLoading.lock();

		switch (m_loadList[group].type)
		{
		case PreloadManager::SPRITE:
			CoInitialize(nullptr);
			for (const std::string filename : m_loadList[group].list)
			{
				m_spriteD12List.insert(RESOURCE.LoadSpriteResourceDX12(filename.c_str()));
			}
			CoUninitialize();
			break;
		case PreloadManager::MODEL:
			for (const std::string filename : m_loadList[group].list)
			{
				m_modelList.insert(RESOURCE.LoadModelDX12Resource(filename.c_str()));
			}
			break;
		}
		m_mtLoading.unlock();
		m_loadList[group];
	}
}

/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
PreloadManager::~PreloadManager()
{
	for (auto& group : m_loadList)
	{
		if (group.second.thread.joinable()) group.second.thread.join();
	}
}

