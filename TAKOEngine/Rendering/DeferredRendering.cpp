#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "LightManager.h"
#include "DeferredRendering.h"

//������
void DeferredRendering::Initialize()
{
	ID3D11Device* device = T_GRAPHICS.GetDevice();

	sprite = std::make_unique<Sprite>(device);
}

//MRT�ݒ�
void DeferredRendering::SetDeferredRTV()
{
	//Deferred Rendering�p
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Normal)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);
	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Position)->Clear(T_GRAPHICS.GetDeviceContext(), 0.2f, 0.2f, 0.2f, 1);

	ID3D11RenderTargetView* rtv[] =
	{
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Normal)->GetRenderTargetView(),
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Position)->GetRenderTargetView(),
	};

	T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)
		->SetRenderTargets(T_GRAPHICS.GetDeviceContext(), 3, rtv);
}

//�`��
void DeferredRendering::Render()
{
	// �`��R���e�L�X�g�ݒ�
	RenderContext rc;
	rc.camera        = &Camera::Instance();
	rc.deviceContext = T_GRAPHICS.GetDeviceContext();
	rc.renderState   = T_GRAPHICS.GetRenderState();

	// ���C�g�̏����l�ߍ���
	LightManager::Instance().PushRenderContext(rc);

	//DeferredRendering�pSRV�ݒ�
	rc.deferredData.normal   = T_GRAPHICS.GetFrameBuffer(FrameBufferId::Normal)->GetShaderResourceView().Get();
	rc.deferredData.position = T_GRAPHICS.GetFrameBuffer(FrameBufferId::Position)->GetShaderResourceView().Get();

	//�`��
	SpriteShader* shader = T_GRAPHICS.GetSpriteShader(SpriteShaderId::Deferred);
	shader->Begin(rc);

	sprite->SetShaderResourceView(
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetShaderResourceView().Get(),
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetWidth(),
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetHeight());

	sprite->Update(
		0, 0,
		T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
		0, 0,
		T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
		0,
		1, 1, 1, 1);

	shader->Draw(rc, sprite.get());
	shader->End(rc);
}

//GUI
void DeferredRendering::DrawDebugGUI()
{
	if (ImGui::TreeNode("G-Buffer"))
	{
		ImGui::Text("texture");
		ImGui::Image(T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetShaderResourceView().Get(), { 1280 / 4,720 / 4 });
		ImGui::Image(T_GRAPHICS.GetFrameBuffer(FrameBufferId::Normal)->GetShaderResourceView().Get(), { 1280 / 4,720 / 4 });
		ImGui::Image(T_GRAPHICS.GetFrameBuffer(FrameBufferId::Position)->GetShaderResourceView().Get(), { 1280 / 4,720 / 4 });
		ImGui::SameLine();
		ImGui::TreePop();
	}
}