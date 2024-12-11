#pragma once

#include <memory>

#include "TAKOEngine/Tool/Singleton.h"

#include "TAKOEngine/Rendering/Sprite.h"
#include "TAKOEngine/Rendering/TextSprite.h"

#include "GameObject/GameObject.h"

class Dialog : public Singleton<Dialog>
{
	friend class Singleton<Dialog>;
private:
	Dialog();
	~Dialog() = default;
public:
	void Update(float elapsedTime);
	void Render();

	void SetName(const char* n) { name = n; timer = 0.0f; Show(); };
	void SetText(const char* t) { text = t; done = false; };
	void SetScale(float s) { scale = s; };
	void SetNameColor(DirectX::XMFLOAT4 color) { nameColor = color; }
	void SetFontColor(DirectX::XMFLOAT4 color) { fontColor = color; }
	void SetVisible(bool v) { visible = v; }
	void Show() { SetVisible(true); }
	void Hide() { SetVisible(false); }
	void Clear();
	void SetOwner(GameObject* object) { dialogOwner = object; }

	void Done() { timer = static_cast<float>(text.length()); done = true; };

	bool IsDone() { return done; }

	void DrawDebugGUI();
private:
	std::shared_ptr<Sprite> rectangle;
	std::shared_ptr<Sprite> triangle;

	std::string name;
	std::string text;

	FONT_ID font = FONT_ID::HGpop;
	float scale = 1.0;
	float timer = 0.0f;
	float typeSpeed = 0.05f; //  秒/一文字

	bool done = true;
	bool visible = false;

	GameObject* dialogOwner = nullptr;

	// 色
	DirectX::XMFLOAT4 nameColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 fontColor = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Dialog
	DirectX::XMFLOAT2 dialogPosition;
	DirectX::XMFLOAT2 dialogSize;
	DirectX::XMFLOAT2 dialogPadding = { 10.0f, 10.0f };
	DirectX::XMFLOAT4 dialogColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	int dialogLength = 22;
};
