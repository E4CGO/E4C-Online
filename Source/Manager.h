﻿//! @file Manager.h
//! @note 

#ifndef __INCLUDE_MANAGER_H__
#define __INCLUDE_MANAGER_H__

#include <mutex>
#include <vector>

template<typename T>
class Manager
{
public:
	// アイテムを登録する
	virtual T* Register(T* item)
	{
		std::lock_guard<std::mutex> lock(m_mut);
		items.emplace_back(item);
		return item;
	}

	// アイテムの登録を解除する
	virtual void Remove(T* item)
	{
		typename std::vector<T*>::iterator it = std::find(items.begin(), items.end(), item);
		if (it != items.end())
		{
			items.erase(it);
			delete item;
		}
	}

	// 登録済みのアイテムを全削除する
	virtual void Clear()
	{
		for (T* item : items)
		{
			delete item;
		}
		items.clear();
	}

	T* Replace(int index, T* item)
	{
		delete items.at(index);
		items.at(index) = item;
		return item;
	}

	T* Get(int index) { return items.at(index); }

	int Count() { return static_cast<int>(items.size()); }

	std::vector<T*>& GetAll() { return items; }

protected:
	std::mutex m_mut;
	std::vector<T*> items;
};

#endif // !__INCLUDE_MANAGER_H__