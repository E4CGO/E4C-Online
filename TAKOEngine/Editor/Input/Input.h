#pragma once

#include <DirectXMath.h>
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <Windows.h>
#include <Xinput.h>

#include <unordered_map>

enum class GAME_PAD_BTN
{
	A,
	B,
	X,
	Y,
	BACK,
	START,
	LSTICK,
	RSTICK,
	LSHOULDER,
	RSHOULDER,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	LTRIGGER,
	RTRIGGER,
};

class Input
{
private:
	Input() = default;
	~Input();
public:
	// インスタンス
	static Input& Instance()
	{
		static Input instance;
		return instance;
	}
	void Init(HWND hWnd);
	void Update();
	void ResigterKey(int key);

	bool KeyDown(int k);
	bool KeyPress(int k);
	bool KeyUp(int k);

	// Mouse
	const DirectX::XMFLOAT2& GetMousePos() const { return MouseData.Pos; }
	const DirectX::XMFLOAT2& GetMouseWinPos()const { return MouseData.WinPos; };
	void UpdateMouseScroll(WPARAM& wParam);
	float GetMouseScrollX();
	float GetMouseScrollY();

	float GetMouseDeltaX();
	float GetMouseDeltaY();
	void KeepCursorCenter();

	void SetCursorPosition(const POINT& p);
	void SetCursorPosition(float x, float y);

	//void SetCursorWinPosition(const POINT& p);
	void SetCursorWinPosition(float x, float y);

	void SetCursorWinPositionX(float x);
	void SetCursorWinPositionY(float y);

	// GamePad
	const DirectX::XMFLOAT2& GetGamePadLAxis() const { return gamePadData.L_Axis; }
	const DirectX::XMFLOAT2& GetGamePadRAxis() const { return gamePadData.R_Axis; }
	bool GamePadKeyDown(GAME_PAD_BTN button);
	bool GamePadKeyPress(GAME_PAD_BTN button);
	bool GamePadKeyUp(GAME_PAD_BTN button);
private:
	void UpdateKeyboard();
	void UpdateGamePad();
	void UpdateMouse();
private:
	HWND hWnd;

	bool ScrollFlag = false;

	std::unordered_map<int, char> keysState;

	// Mouse
	struct Mouse {
		DirectX::XMFLOAT2 Pos = {};
		DirectX::XMFLOAT2 WinPos = {};
		DirectX::XMFLOAT2 Delta = {};
		DirectX::XMINT2 Scroll = {};
	} MouseData;

	// GamePad
	struct GamePad {
		int slot = 0;
		DirectX::XMFLOAT2 L_Axis = {};
		DirectX::XMFLOAT2 R_Axis = {};
		float LTrigger = 0;
		float RTrigger = 0;
		std::unordered_map<GAME_PAD_BTN, char> keysState;
	} gamePadData;
};