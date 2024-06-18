#pragma once

#include <vector>

template<typename T>
class Manager
{
public:
	// ƒAƒCƒeƒ€‚ğ“o˜^‚·‚é
	virtual T* Register(T* item)
	{
		items.emplace_back(item);
		return item;
	}

	// ƒAƒCƒeƒ€‚Ì“o˜^‚ğ‰ğœ‚·‚é
	virtual void Remove(T* item)
	{
		typename std::vector<T*>::iterator it = std::find(items.begin(), items.end(), item);
		if (it != items.end())
		{
			items.erase(it);
			delete item;
		}
	}

	// “o˜^Ï‚İ‚ÌƒAƒCƒeƒ€‚ğ‘Síœ‚·‚é
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

	T* get(int index) { return items.at(index); }

	int Count() { return static_cast<int>(items.size()); }

	std::vector<T*>& GetAll() { return items; }
protected:
	std::vector<T*> items;
};
