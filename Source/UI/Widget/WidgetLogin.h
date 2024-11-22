#pragma once

#include "TAKOEngine/Network/HttpRequest.h"
#include "TAKOEngine/GUI/Widget.h"

class WidgetLogin : public Widget
{
public:
	WidgetLogin();
	~WidgetLogin() {};

	virtual void Update(float elapsedTime);
	virtual void Render(const RenderContext& rc);
	virtual void RenderDX12(const RenderContextDX12& rc);

	int GetState() { return state; }
public:
	enum STATE
	{
		INPUT,
		LOADING,
		SUCCESS,
		FAIL
	};
private:
	int state = STATE::INPUT;
	char username[64];
	char password[32];

	HttpRequest* xhr = nullptr;
};
