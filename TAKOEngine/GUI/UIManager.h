#pragma once

#include "TAKOEngine/Tool/Singleton.h"
#include "TAKOEngine/GUI/Widget.h"

#include "GameObject/ObjectManager.h"

class UIManager : public ObjectManager<Widget>, public Singleton<UIManager>
{
	friend class Singleton<UIManager>;
protected:
	UIManager() = default;
	~UIManager() = default;

public:
	// XVˆ—
	void Update(float elapsedTime) override;

	void SetCursorName(LPWSTR lpCursorName);
private:
	LPWSTR lpCursorName;
};

#define UI UIManager::Instance()