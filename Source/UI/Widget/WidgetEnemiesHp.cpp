#include "WidgetEnemiesHp.h"

#include "TAKOEngine/Rendering/ResourceManager.h"
#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Editor/Camera/CameraManager.h"

#include "GameObject/Character/Enemy/EnemyManager.h"

WidgetEnemiesHp::WidgetEnemiesHp(DirectX::XMFLOAT2 size)
{
	// フレームバッファマネージャー
	m_frameBuffer = T_GRAPHICS.GetFrameBufferManager();
	
	if (T_GRAPHICS.isDX11Active) this->gauge = RESOURCE.LoadSpriteResource(""); // 長方形
	else this->m_gaugeDX12 = RESOURCE.LoadSpriteResourceDX12("");

	this->m_size = size; // ゲージサイズ
}
WidgetEnemiesHp::WidgetEnemiesHp(float width, float height) :WidgetEnemiesHp(DirectX::XMFLOAT2{ width, height }) {}

void WidgetEnemiesHp::Render(const RenderContext& rc)
{
	// ビューボード
	D3D11_VIEWPORT viewport;

	UINT numViewports = 1;
	rc.deviceContext->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// カメラ
	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();

	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		if (!enemy->IsShowHp()) continue;
		float hp = static_cast<float>(enemy->GetHp());
		float maxHp = static_cast<float>(enemy->GetMaxHp());
		if (hp >= maxHp) continue;

		DirectX::XMFLOAT3 pos = enemy->GetPosition();

		float dot = XMFLOAT3Dot(front, pos - eye);
		if (dot < 0.0f) continue;

		pos.y += enemy->GetHeight() * 1.4f;
		pos = T_GRAPHICS.GetScreenPosition(pos, viewport, View, Projection, World);

		this->gauge->Render(
			rc.deviceContext,
			pos.x - (m_size.x * 0.5f), pos.y - (m_size.y * 0.5f), 0,
			m_size.x, m_size.y,
			0.0f,
			0.0f, 0.0f, 0.0f, 0.5f
		);

		this->gauge->Render(
			rc.deviceContext,
			pos.x - (m_size.x * 0.5f) + 1.0f, pos.y - (m_size.y * 0.5f) + 1.0f, 0,
			(m_size.x - 2.0f) * (hp / maxHp), m_size.y - 2.0f,
			0.0f,
			0.7f, 0.0f, 0.0f, 1.0f
		);
	}
}

void WidgetEnemiesHp::RenderDX12(const RenderContextDX12& rc)
{
	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&CameraManager::Instance().GetCamera()->GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// カメラ
	DirectX::XMFLOAT3 front = CameraManager::Instance().GetCamera()->GetFront();
	DirectX::XMFLOAT3 eye = CameraManager::Instance().GetCamera()->GetEye();

	for (Enemy*& enemy : ENEMIES.GetAll())
	{
		if (!enemy->IsShowHp()) continue;
		float hp = static_cast<float>(enemy->GetHp());
		float maxHp = static_cast<float>(enemy->GetMaxHp());
		if (hp >= maxHp) continue;

		DirectX::XMFLOAT3 pos = enemy->GetPosition();

		float dot = XMFLOAT3Dot(front, pos - eye);
		if (dot < 0.0f) continue;

		pos.y += enemy->GetHeight() * 1.4f;
		pos = T_GRAPHICS.GetScreenPosition(pos, m_frameBuffer->GetViewport(), View, Projection, World);

		m_gaugeDX12->Begin(rc);
		m_gaugeDX12->Draw(
			pos.x - (m_size.x * 0.5f), pos.y - (m_size.y * 0.5f),
			m_size.x, m_size.y,
			0.0f,
			0.0f, 0.0f, 0.0f, 0.5f);
		m_gaugeDX12->End(rc.d3d_command_list);

		m_gaugeDX12->Begin(rc);
		m_gaugeDX12->Draw(
			pos.x - (m_size.x * 0.5f) + 1.0f, pos.y - (m_size.y * 0.5f) + 1.0f,
			(m_size.x - 2.0f) * (hp / maxHp), m_size.y - 2.0f,
			0.0f,
			0.7f, 0.0f, 0.0f, 1.0f);
		m_gaugeDX12->End(rc.d3d_command_list);
	}
}