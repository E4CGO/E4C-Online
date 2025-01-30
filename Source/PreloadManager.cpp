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
		if (f.fail()) return;
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
	m_thread = std::thread(&PreloadManager::LoadThread, this);
}
/**************************************************************************//**
	@brief		ロードスレッド関数
	@param[in]	group ロードグループネーム
*//***************************************************************************/
void PreloadManager::LoadThread()
{
	for (const std::string& group : m_groupOrder)
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		for (const std::string filename : m_loadList[group].list)
		{
			if (m_flgEnd) {
				CoUninitialize();
				return;
			}
			while (m_lock)
			{
				Sleep(10); // ロック
			}
			switch (m_loadList[group].type)
			{
			case PreloadManager::SPRITE:
				m_spriteD12List.insert(RESOURCE.LoadSpriteResourceDX12(filename.c_str()));
				break;
			case PreloadManager::MODEL:
				m_modelList.insert(RESOURCE.LoadModelDX12Resource(filename.c_str()));
				break;
			}
		}
		m_loadList[group].done = true;
		CoUninitialize();
	}
}

/**************************************************************************//**
	@brief	デストラクタ
*//***************************************************************************/
PreloadManager::~PreloadManager()
{
	m_flgEnd = true;
	if (m_thread.joinable()) m_thread.join();
}

