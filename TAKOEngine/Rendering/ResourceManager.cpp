#include "Graphics.h"
#include "ResourceManager.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* filename)
{
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

std::shared_ptr<Sprite> ResourceManager::LoadSpriteResource(const char* filename)
{
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