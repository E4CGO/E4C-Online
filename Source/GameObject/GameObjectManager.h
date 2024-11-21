#include <set>
#include "Manager.h"
#include "TAKOEngine/Tool/Singleton.h"

#include "GameObject.h"

class GameObjectManager : public Manager<GameObject>, public Singleton<GameObjectManager>
{
public:
	virtual void Update(float elapsedTime)
	{
		for (GameObject* item : removes)
		{
			typename std::vector<GameObject*>::iterator it = std::find(this->items.begin(), this->items.end(), item);
			if (it != this->items.end())
			{
				this->items.erase(it);
			}
			delete item;
		}
		removes.clear();

		size_t size = this->items.size();
		for (size_t i = 0; i < size; i++)
		{
			if (this->items.size() <= i) break;
			this->items.at(i)->Update(elapsedTime);
		}
	}
	virtual void Render(const RenderContext& rc)
	{
		for (GameObject* item : this->items)
		{
			item->Render(rc);
		}
	}

	void MoveToEnd(GameObject* item)
	{
		typename std::vector<GameObject*>::iterator it = std::find(this->items.begin(), this->items.end(), item);
		if (it != this->items.end())
		{
			this->items.erase(it);
			this->Register(item);
		}
	}
	// アイテムの登録を解除する
	void Remove(GameObject* item)
	{
		removes.insert(item);
	}
	// リサイズ
	void Resize(int size)
	{
		int i = 0;
		for (GameObject* item : this->items)
		{
			i++;
			if (i > size) removes.insert(item);
		}
	}
	// デバッグ情報の表示
	void DrawDebugGUI()
	{
		for (GameObject* item : this->items)
		{
			item->DrawDebugGUI();
		}
	}
	// デバッグプリミティブの表示
	void DrawDebugPrimitive()
	{
		for (GameObject* item : this->items)
		{
			item->DrawDebugPrimitive();
		}
	}

protected:
	std::set<GameObject*> removes;
};