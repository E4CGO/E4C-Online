#include <profiler.h>
#include "Scene/SceneTest.h"
#include "Map/MapTileManager.h"

void SceneTest::Initialize()
{
	stage = new MapTile("Data/Model/Stage/BigMap.glb", 1000);
	stage->Update(0);
	MAPTILES.Register(stage);

	player = std::make_unique<Knight>();
	player->SetPosition({ 5, -50, 5 });
	player->GetStateMachine()->ChangeState(static_cast<int>(Player::State::Idle));
	//knight = std::make_unique<ModelObject>("Data/Model/Character/Knight.glb");
	//knight->SetAnimation(22, true, 0.0f);
	//knight->SetPosition({ 1.08f, 0.0f, 2.12f });
	//knight->SetAngle({ 0.0f, -1.22f, 0.0f });
	//knight->GetModel()->FindNode("1H_Sword_Offhand")->visible = false;
	//knight->GetModel()->FindNode("2H_Sword")->visible = false;
	//knight->GetModel()->FindNode("Badge_Shield")->visible = false;
	//knight->GetModel()->FindNode("Round_Shield")->visible = false;
	//knight->GetModel()->FindNode("Spike_Shield")->visible = false;

	// ��
	LightManager::Instance().SetAmbientColor({ 0, 0, 0, 0 });
	Light* dl = new Light(LightType::Directional);
	dl->SetDirection({ 0.0f, -0.503f, -0.864f });
	LightManager::Instance().Register(dl);

	// �J�����ݒ�
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),							// ��p
		T_GRAPHICS.GetScreenWidth() / T_GRAPHICS.GetScreenHeight(),	// ��ʃA�X�y�N�g��
		0.1f,														// �j�A�N���b�v
		10000.0f													// �t�@�[�N���b�v
	);
	camera.SetLookAt(
		{ 0, 5.0f, 10.0f },	// ���_
		{ 0, 0, 0 },	// �����_
		{ 0, 0.969f, -0.248f } // ��x�N�g��
	);

	cameraController = std::make_unique<ThridPersonCameraController>();
	cameraController->SyncCameraToController(camera);
	//cameraController->SetEnable(false);
	cameraController->SetEnable(true);
	cameraController->SetPlayer(player.get());
}

void SceneTest::Finalize()
{
	LightManager::Instance().Clear();
	MAPTILES.Clear();
}

// �X�V����
void SceneTest::Update(float elapsedTime)
{
	cameraController->Update(elapsedTime);
	cameraController->SyncContrllerToCamera(camera);

	MAPTILES.Update(elapsedTime);

	player->Update(elapsedTime);
}

// �`�揈��
void SceneTest::Render()
{
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Display)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

	// �`��R���e�L�X�g�ݒ�
	RenderContext rc;
	rc.camera = &camera;
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState = T_GRAPHICS.GetRenderState();

	// ���C�g�̏����l�ߍ���
	LightManager::Instance().PushRenderContext(rc);

	// �`��
	//knight->Render(rc);
	player->Render(rc);
	MAPTILES.Render(rc);

	ProfileDrawUI();
}