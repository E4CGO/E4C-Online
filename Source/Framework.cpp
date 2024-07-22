#include <memory>
#include <sstream>
#include <imgui.h>

#include "Framework.h"

#include "TAKOEngine/Tool/ImGuiRenderer.h"
#include "TAKOEngine/Effects/EffectManager.h"

#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#include "Scene/SceneManager.h"

#include "GameData.h"

// ���������Ԋu�ݒ�
static const int syncInterval = 1;
extern bool gPause;

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
{
	TentacleLib::SetSyncInterval(syncInterval);
	TentacleLib::SetShowFPS(true);

	// IMGUI������
	ImGuiRenderer::Initialize(hWnd, T_GRAPHICS.GetDevice(), T_GRAPHICS.GetDeviceContext());

	// �l�b�g���[�N
	Network::Initialize();
	// IP���擾
	char address[256];
	Network::GetIpAddress(address, sizeof(address));
	GAME_DATA.SetIp(address);

	// �G�t�F�N�g�}�l�[�W���[������
	EFFECTS.Initialize();

	// �V�[��������
	SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame("nagi", "192.168.0.236", "7000", new HostNetworkController)));
}

// �f�X�g���N�^
Framework::~Framework()
{
	T_GRAPHICS.WaitIdle();

	SceneManager::Instance().Clear();

	// �G�t�F�N�g�}�l�[�W���[�I����
	EFFECTS.Finalize();

	// IMGUI�I��
	ImGuiRenderer::Finalize();

	Network::Finalize();

}

// �X�V����
void Framework::Update(float elapsedTime)
{
	if (T_INPUT.KeyDown(VK_F1)) DX12API = !DX12API;
	// �V�[���X�V����
	SceneManager::Instance().Update(elapsedTime);
}

// �`�揈��
void Framework::Render(float elapsedTime)
{
	// �ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
	// �����A�N�Z�X���Ȃ��悤�ɔr�����䂷
	std::lock_guard<std::mutex> lock(T_GRAPHICS.GetMutex());

	if (!DX12API)
	{
		ID3D11DeviceContext* dc = T_GRAPHICS.GetDeviceContext();

		// IMGUI�����J�n
		ImGuiRenderer::NewFrame();

		// �����_�[�^�[�Q�b�g�ݒ�
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(dc);

		// �V�[���`�揈��
		SceneManager::Instance().Render();

		// IMGUI�`��
		ImGuiRenderer::Render(dc);

		// ��ʕ\��
		TentacleLib::Draw();
	}
	else
	{
		T_GRAPHICS.Execute();
		SceneManager::Instance().RenderDX12();
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// TODO: Move update
			TentacleLib::Update();
			float elapsedTime = TentacleLib::timer.Delta();

			if (gPause) elapsedTime = 0.0f;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}

	TentacleLib::Finalize();

	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGuiRenderer::HandleMessage(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_MOUSEWHEEL:
	{
		T_INPUT.UpdateMouseScroll(wParam);
	}
	break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		//timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		//timer.Start();
		break;

	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}