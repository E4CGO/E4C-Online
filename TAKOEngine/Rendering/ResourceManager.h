#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "TAKOEngine/Rendering/Model/ModelResource.h"
#include "TAKOEngine/Rendering/Sprite.h"

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

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);
	std::shared_ptr<Sprite> LoadSpriteResource(const char* filename);

private:
	using ModelMap = std::unordered_map<std::string, std::weak_ptr<ModelResource>>;
	ModelMap models;
	using SpriteMap = std::unordered_map<std::string, std::weak_ptr<Sprite>>;
	SpriteMap sprites;
};

#define RESOURCE ResourceManager::Instance()