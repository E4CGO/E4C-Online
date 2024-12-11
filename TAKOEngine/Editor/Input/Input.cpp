#include "Input.h"

#include "TAKOEngine/Runtime/tentacle_lib.h"

#include <windowsx.h>
#include "TAKOEngine/Network/WinSock2Wrapper.h"
#include <mmsystem.h> 
Input::~Input()
{
}

void Input::ResigterKey(int key)
{
	keysState[key] = 0;
}

void Input::Init(HWND hWnd) {
	this->hWnd = hWnd;

	POINT p;
	if (GetCursorPos(&p))
	{
		MouseData.Pos.x = static_cast<float>(p.x);
		MouseData.Pos.y = static_cast<float>(p.y);
	}

	// 初期化
	gamePadData.keysState[GAME_PAD_BTN::A] = 0;
	gamePadData.keysState[GAME_PAD_BTN::B] = 0;
	gamePadData.keysState[GAME_PAD_BTN::X] = 0;
	gamePadData.keysState[GAME_PAD_BTN::Y] = 0;
	gamePadData.keysState[GAME_PAD_BTN::BACK] = 0;
	gamePadData.keysState[GAME_PAD_BTN::START] = 0;
	gamePadData.keysState[GAME_PAD_BTN::LSTICK] = 0;
	gamePadData.keysState[GAME_PAD_BTN::RSTICK] = 0;
	gamePadData.keysState[GAME_PAD_BTN::LSHOULDER] = 0;
	gamePadData.keysState[GAME_PAD_BTN::RSHOULDER] = 0;
	gamePadData.keysState[GAME_PAD_BTN::DPAD_UP] = 0;
	gamePadData.keysState[GAME_PAD_BTN::DPAD_DOWN] = 0;
	gamePadData.keysState[GAME_PAD_BTN::DPAD_LEFT] = 0;
	gamePadData.keysState[GAME_PAD_BTN::DPAD_RIGHT] = 0;
	gamePadData.keysState[GAME_PAD_BTN::LTRIGGER] = 0;
	gamePadData.keysState[GAME_PAD_BTN::RTRIGGER] = 0;
}

void Input::Update()
{
	// キーボード
	UpdateKeyboard();

	// ゲームパッド
	UpdateGamePad();

	// マウス
	UpdateMouse();
}
void Input::UpdateKeyboard()
{
	BYTE key[256];
	GetKeyboardState(key);
	for (auto& [k, state] : keysState)
	{
		state = (state << 1) & 3;

		if (key[k] & 0x80)
		{
			state |= 1;
		}
	}
}
void Input::UpdateGamePad()
{
	// ボタンニューフレーム
	for (auto& [button, state] : gamePadData.keysState)
	{
		state = (state << 1) & 3;
	}

	// ボタン情報取得
	XINPUT_STATE xinputState;
	if (XInputGetState(gamePadData.slot, &xinputState) == ERROR_SUCCESS)
	{
		// 普通ゲームパッド
		XINPUT_GAMEPAD& pad = xinputState.Gamepad;
		if (pad.wButtons & XINPUT_GAMEPAD_DPAD_UP)					gamePadData.keysState[GAME_PAD_BTN::DPAD_UP] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)				gamePadData.keysState[GAME_PAD_BTN::DPAD_RIGHT] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)				gamePadData.keysState[GAME_PAD_BTN::DPAD_DOWN] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)				gamePadData.keysState[GAME_PAD_BTN::DPAD_LEFT] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_A)						gamePadData.keysState[GAME_PAD_BTN::A] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_B)						gamePadData.keysState[GAME_PAD_BTN::B] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_X)						gamePadData.keysState[GAME_PAD_BTN::X] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_Y)						gamePadData.keysState[GAME_PAD_BTN::Y] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_START)					gamePadData.keysState[GAME_PAD_BTN::START] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_BACK)						gamePadData.keysState[GAME_PAD_BTN::BACK] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)				gamePadData.keysState[GAME_PAD_BTN::LSTICK] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)				gamePadData.keysState[GAME_PAD_BTN::RSTICK] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)			gamePadData.keysState[GAME_PAD_BTN::LSHOULDER] |= 1;
		if (pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)			gamePadData.keysState[GAME_PAD_BTN::RSHOULDER] |= 1;
		if (pad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	gamePadData.keysState[GAME_PAD_BTN::LTRIGGER] |= 1;
		if (pad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	gamePadData.keysState[GAME_PAD_BTN::RTRIGGER] |= 1;

		// Axis
		if ((pad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(pad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && pad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			pad.sThumbLX = 0;
			pad.sThumbLY = 0;
		}
		if ((pad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(pad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && pad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			pad.sThumbRX = 0;
			pad.sThumbRY = 0;
		}
		gamePadData.LTrigger = static_cast<float>(pad.bLeftTrigger) / 255.0f;
		gamePadData.LTrigger = static_cast<float>(pad.bRightTrigger) / 255.0f;
		gamePadData.L_Axis = {
			static_cast<float>(pad.sThumbLX) / static_cast<float>(0x8000),
			static_cast<float>(pad.sThumbLY) / static_cast<float>(0x8000)
		};
		gamePadData.R_Axis = {
			static_cast<float>(pad.sThumbRX) / static_cast<float>(0x8000),
			static_cast<float>(pad.sThumbRY) / static_cast<float>(0x8000)
		};
	}
	else
	{
		// XInputで入力情報が取得出来なかった場合はWindowsAPIで取得する
		JOYINFOEX joyInfo;
		joyInfo.dwSize = sizeof(JOYINFOEX);
		joyInfo.dwFlags = JOY_RETURNALL;	// 全ての情報を取得
		if (joyGetPosEx(gamePadData.slot, &joyInfo) == JOYERR_NOERROR)
		{
			// 製品IDをチェックしてPS4コントローラーだけ対応する
			static const WORD PS4_PID = 1476;

			JOYCAPS joy_caps;
			if (joyGetDevCaps(gamePadData.slot, &joy_caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
			{
				// 十字キー
				switch (joyInfo.dwPOV)
				{
				case 0xFFFF:break;	// 65535：入力なし
				case 0x0000: gamePadData.keysState[GAME_PAD_BTN::DPAD_UP] |= 1; break;		// 0: 上
				case 0x2328: gamePadData.keysState[GAME_PAD_BTN::DPAD_RIGHT] |= 1; break;	// 9000: 右
				case 0x4650: gamePadData.keysState[GAME_PAD_BTN::DPAD_DOWN] |= 1; break;	// 18000: 下
				case 0x6978: gamePadData.keysState[GAME_PAD_BTN::DPAD_LEFT] |= 1; break;	// 27000: 左

				case 0x1194: // 4500: 右上
					gamePadData.keysState[GAME_PAD_BTN::DPAD_RIGHT] |= 1;
					gamePadData.keysState[GAME_PAD_BTN::DPAD_UP] |= 1;
					break;
				case 0x34bc: // 13500: 右下
					gamePadData.keysState[GAME_PAD_BTN::DPAD_RIGHT] |= 1;
					gamePadData.keysState[GAME_PAD_BTN::DPAD_DOWN] |= 1;
					break;
				case 0x57e4: // 22500: 左下
					gamePadData.keysState[GAME_PAD_BTN::DPAD_LEFT] |= 1;
					gamePadData.keysState[GAME_PAD_BTN::DPAD_DOWN] |= 1;
					break;
				case 0x7b0c: // 31500: 左上
					gamePadData.keysState[GAME_PAD_BTN::DPAD_LEFT] |= 1;
					gamePadData.keysState[GAME_PAD_BTN::DPAD_UP] |= 1;
					break;
				}

				if (joy_caps.wPid == PS4_PID)
				{
					// ボタン情報
					if (joyInfo.dwButtons & JOY_BUTTON1)  gamePadData.keysState[GAME_PAD_BTN::X] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON2)  gamePadData.keysState[GAME_PAD_BTN::A] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON3)  gamePadData.keysState[GAME_PAD_BTN::B] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON4)  gamePadData.keysState[GAME_PAD_BTN::Y] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON5)  gamePadData.keysState[GAME_PAD_BTN::LSHOULDER] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON6)  gamePadData.keysState[GAME_PAD_BTN::RSHOULDER] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON7)  gamePadData.keysState[GAME_PAD_BTN::LTRIGGER] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON8)  gamePadData.keysState[GAME_PAD_BTN::RTRIGGER] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON9)  gamePadData.keysState[GAME_PAD_BTN::BACK] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON10) gamePadData.keysState[GAME_PAD_BTN::START] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON11) gamePadData.keysState[GAME_PAD_BTN::LSTICK] |= 1;
					if (joyInfo.dwButtons & JOY_BUTTON12) gamePadData.keysState[GAME_PAD_BTN::RSTICK] |= 1;
					//if (joyInfo.dwButtons & JOY_BUTTON13) newButtonState |= BTN_?;	// PS
					//if (joyInfo.dwButtons & JOY_BUTTON14) newButtonState |= BTN_?;	// Touch

					// 左スティック
					gamePadData.L_Axis = {
						static_cast<int>(joyInfo.dwXpos - 0x7FFF) / static_cast<float>(0x8000),
						-static_cast<int>(joyInfo.dwYpos - 0x7FFF) / static_cast<float>(0x8000)
					};

					// 右スティック
					gamePadData.R_Axis = {
						static_cast<int>(joyInfo.dwZpos - 0x7FFF) / static_cast<float>(0x8000),
						-static_cast<int>(joyInfo.dwRpos - 0x7FFF) / static_cast<float>(0x8000)
					};

					// LRトリガー
					gamePadData.LTrigger = static_cast<float>(joyInfo.dwVpos) / static_cast<float>(0xFFFF);
					gamePadData.RTrigger = static_cast<float>(joyInfo.dwUpos) / static_cast<float>(0xFFFF);

					if (gamePadData.L_Axis.x > -0.25f && gamePadData.L_Axis.x < 0.25f) gamePadData.L_Axis.x = 0.0f;
					if (gamePadData.L_Axis.y > -0.25f && gamePadData.L_Axis.y < 0.25f) gamePadData.L_Axis.y = 0.0f;
					if (gamePadData.R_Axis.x > -0.25f && gamePadData.R_Axis.x < 0.25f) gamePadData.R_Axis.x = 0.0f;
					if (gamePadData.R_Axis.y > -0.25f && gamePadData.R_Axis.y < 0.25f) gamePadData.R_Axis.y = 0.0f;
				}
			}
		}
	}
}
void Input::UpdateMouse()
{
	POINT p;
	if (GetCursorPos(&p))
	{
		MouseData.Delta.x = p.x - MouseData.Pos.x;
		MouseData.Delta.y = p.y - MouseData.Pos.y;

		MouseData.Pos.x = static_cast<float>(p.x);
		MouseData.Pos.y = static_cast<float>(p.y);

		// ウィンドウズ内座標
		ScreenToClient(this->hWnd, &p);
		MouseData.WinPos.x = static_cast<float>(p.x);
		MouseData.WinPos.y = static_cast<float>(p.y);
	}

	if (ScrollFlag)
	{
		ScrollFlag = false;
	}
	else
	{
		MouseData.Scroll.y = 0;
	}
}

void Input::SetCursorPosition(const POINT& p)
{
	SetCursorPos(static_cast<int>(p.x), static_cast<int>(p.y));
	MouseData.Pos.x = static_cast<float>(p.x);
	MouseData.Pos.y = static_cast<float>(p.y);
}

void Input::SetCursorPosition(float x, float y)
{
	SetCursorPos(static_cast<int>(x), static_cast<int>(y));
	MouseData.Pos.x = x;
	MouseData.Pos.y = y;
}

void Input::SetCursorWinPosition(float x, float y)
{
	POINT p{ static_cast<LONG>(x), static_cast<LONG>(y) };
	ClientToScreen(this->hWnd, &p);
	Input::SetCursorPosition(p);
}

void Input::SetCursorWinPositionX(float x)
{
	Input::SetCursorWinPosition(x, MouseData.WinPos.y);
}
void Input::SetCursorWinPositionY(float y)
{
	Input::SetCursorWinPosition(MouseData.WinPos.x, y);
}

void Input::KeepCursorCenter()
{
	Input::SetCursorWinPosition(SCREEN_W / 2.0f, SCREEN_H / 2.0f);
}

bool Input::KeyDown(int k) { if (!TentacleLib::isFocus()) return false; return keysState[k] == 1; }
bool Input::KeyPress(int k) { if (!TentacleLib::isFocus()) return false; return keysState[k] & 1; }
bool Input::KeyUp(int k) { if (!TentacleLib::isFocus()) return false; return keysState[k] == 2; }

float Input::GetMouseDeltaX() { return MouseData.Delta.x; };
float Input::GetMouseDeltaY() { return MouseData.Delta.y; };

void Input::UpdateMouseScroll(WPARAM& wParam)
{
	ScrollFlag = true;
	MouseData.Scroll.y = GET_WHEEL_DELTA_WPARAM(wParam);
}
float Input::GetMouseScrollX()
{
	return static_cast<float>(MouseData.Scroll.x);
}
float Input::GetMouseScrollY()
{
	return static_cast<float>(MouseData.Scroll.y) / WHEEL_DELTA;
}

bool Input::GamePadKeyDown(GAME_PAD_BTN button) { if (!TentacleLib::isFocus()) return false; return gamePadData.keysState[button] == 1; }
bool Input::GamePadKeyPress(GAME_PAD_BTN button) { if (!TentacleLib::isFocus()) return false; return gamePadData.keysState[button] & 1; }
bool Input::GamePadKeyUp(GAME_PAD_BTN button) { if (!TentacleLib::isFocus()) return false; return gamePadData.keysState[button] == 2; }