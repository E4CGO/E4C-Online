#include "Graphics.h"
#include "ResourceManager.h"

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
	// スプライト検索
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