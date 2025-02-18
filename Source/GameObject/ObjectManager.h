//! @file ObjectManager.h
//! @note

#ifndef __INCLUDED_OBJECT_MANAGER__
#define __INCLUDED_OBJECT_MANAGER__

#include <set>
#include "Manager.h"

#include "GameObject.h"

template<typename T>
class ObjectManager : public Manager<T>
{
public:
	virtual void Update(float elapsedTime)
	{
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);

		// 安全登録
		m_mutRegister.lock();
		for (T* item : m_pRegisters)
		{
			this->items.emplace_back(item);
		}
		m_pRegisters.clear();
		m_mutRegister.unlock();

		for (T* item : m_pRemoves)
		{
			typename std::vector<T*>::iterator it = std::find(this->items.begin(), this->items.end(), item);
			if (it != this->items.end())
			{
				this->items.erase(it);
			}
			delete item;
		}
		m_pRemoves.clear();

		size_t size = this->items.size();
		for (size_t i = 0; i < size; i++)
		{
			if (this->items.size() <= i) break;
			this->items.at(i)->Update(elapsedTime);
		}
	}
	virtual void Render(const RenderContext& rc)
	{
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);
		for (T* item : this->items)
		{
			item->Render(rc);
		}
	}

	virtual void RenderDX12(const RenderContextDX12& rc)
	{
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);
		for (T* item : this->items)
		{
			item->RenderDX12(rc);
		}
	}

	void MoveToEnd(T* item)
	{
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);
		typename std::vector<T*>::iterator it = std::find(this->items.begin(), this->items.end(), item);
		if (it != this->items.end())
		{
			this->items.erase(it);
			this->Register(item);
		}
	}
	// アイテムの登録を解除する
	void Remove(T* item)
	{
		m_pRemoves.insert(item);
	}

	// リサイズ
	void Resize(int size)
	{
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);
		int i = 0;
		for (T* item : this->items)
		{
			i++;
			if (i > size) m_pRemoves.insert(item);
		}
	}
	// デバッグ情報の表示
	void DrawDebugGUI()
	{
		for (T* item : this->items)
		{
			item->DrawDebugGUI();
		}
	}
	// デバッグプリミティブの表示
	void DrawDebugPrimitive()
	{
		for (T* item : this->items)
		{
			item->DrawDebugPrimitive();
		}
	}
	void Clear() override {
		std::lock_guard<std::mutex> lock(Manager<T>::m_mut);
		Manager<T>::Clear();
		m_pRemoves.clear();
	}

	T* SoftRegister(T* item)
	{
		std::lock_guard<std::mutex> lock(m_mutRegister);
		m_pRegisters.insert(item);
		return item;
	}
protected:
	std::set<T*> m_pRemoves;

	// 安全登録
	std::set<T*> m_pRegisters;
	std::mutex m_mutRegister;
};

#endif // !__INCLUDED_OBJECT_MANAGER__
