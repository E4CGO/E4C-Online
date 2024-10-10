#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "EffectManager.h"

void EffectManager::Initialize()		// ������
{
	// Effekseer�����_������
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
		T_GRAPHICS.GetDevice(),
		T_GRAPHICS.GetDeviceContext(),
		2048
	);

	// Effekseer�}�l�[�W���[����
	effekseerManager = Effekseer::Manager::Create(2048);

	// Effekseer�����_���̊e��ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͒�^�I�Ɉȉ��̐ݒ��OK�j
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// Effekseer���ł̃��[�_�[�̐ݒ�i���ʂȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ��OK�j
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());
	// Effekseer��������W�n�Ōv�Z����
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	effects[EFFECT_IDX::HIT_SLASH_EFFECT] = new Effect("Data/Effects/Hit01.efk");
	effects[EFFECT_IDX::HIT_SHOT_EFFECT] = new Effect("Data/Effects/Hit02.efk");
	effects[EFFECT_IDX::BOMB_EFFECT] = new Effect("Data/Effects/Bomb.efk");
}

void EffectManager::InitializeDX12()		// ������
{
	auto graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(T_GRAPHICS.GetDeviceDX12(), T_GRAPHICS.GetCommandQueue().d3d_command_queue.Get(), 3);

	// Create a manager of effects
	// �G�t�F�N�g�̃}�l�[�W���[�̍쐬
	efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseer�̃��W���[�����Z�b�g�A�b�v����
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	// Create a memory pool
	// �������v�[���̍쐬
	::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Create a command list
	// �R�}���h���X�g�̍쐬
	::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);

	// Sprcify rendering modules
	// �`�惂�W���[���̐ݒ�
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
	// ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	effects[EFFECT_IDX::HIT_SLASH_EFFECT] = new Effect("Data/Effects/Hit01.efk");
	effects[EFFECT_IDX::HIT_SHOT_EFFECT] = new Effect("Data/Effects/Hit02.efk");
	effects[EFFECT_IDX::BOMB_EFFECT] = new Effect("Data/Effects/Bomb.efk");
}

Effect* EffectManager::GetEffect(int index)
{
	if (effects.find(index) == effects.end()) return nullptr;
	return effects[index];
}

void EffectManager::Finalize()
{
	for (std::pair<int, Effect*> effect : effects)
	{
		delete effect.second;
	}
	effects.clear();
}

void EffectManager::Update(float elapsedTime)
{
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// �r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// Effekseer�`��J�n
	effekseerRenderer->BeginRendering();
	// Effekseer�`����s
	// �}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
	// Draw()�֐������s���鏇���Ő���ł�����
	effekseerManager->Draw();
	// Effekseer�`��I��
	effekseerRenderer->EndRendering();
}

void EffectManager::RenderDX12(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// �r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
	efkRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	efkRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	Effekseer::Manager::UpdateParameter updateParameter;
	efkManager->Update(updateParameter);

	// Effekseer�`��J�n
	efkRenderer->BeginRendering();

	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();

	// Effekseer�`����s
	// �}�l�[�W���[�P�ʂŕ`�悷��̂ŕ`�揇�𐧌䂷��ꍇ�̓}�l�[�W���[�𕡐��쐬���A
	// Draw()�֐������s���鏇���Ő���ł�����
	//efkManager->Draw();
	// Effekseer�`��I��
	efkRenderer->EndRendering();
}