//! @file Teleporter.cpp
//! @note

#include "Teleporter.h"

#include "TAKOEngine/Tool/XMFLOAT.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"
#include "Scene/Stage/StageManager.h"
#include "TAKOEngine/GUI/UIManager.h"

#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"
#include "Scene/SceneManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	stage	ステージ参照ポインタ
	@return	なし
*//***************************************************************************/
Teleporter::Teleporter(Stage* stage, Online::OnlineController* onlineController) : m_pStage(stage), m_pOnlineController(onlineController), ModelObject()
{
	m_timer = 0.0f;
	if (T_GRAPHICS.isDX11Active)
	{
		SetShader(ModelShaderId::Portal);

		ID3D11Device* device = T_GRAPHICS.GetDevice();

		HRESULT hr = S_OK;
		{
			using namespace DirectX;

			m_mesh.vertices = m_defaultVertices;

			// 頂点バッファを作成するための設定オプション
			D3D11_BUFFER_DESC buffer_desc = {};
			buffer_desc.ByteWidth = sizeof(ModelResource::Vertex) * 4; // 4頂点
			buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData = {};
			vertexData.pSysMem = m_mesh.vertices.data();

			// 頂点バッファオブオブジェクトの生成
			hr = device->CreateBuffer(&buffer_desc, &vertexData, m_mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}

		{
			unsigned int indices[] = {
				0, 1, 2,
				2, 1, 3
			};

			D3D11_BUFFER_DESC indexBufferDesc = {};
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(indices);
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indexData = {};
			indexData.pSysMem = indices;

			hr = device->CreateBuffer(&indexBufferDesc, &indexData, m_mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		}
		m_mesh.material = new ModelResource::Material;

		// ダミーテクスチャ生成
		D3D11_TEXTURE2D_DESC desc;
		hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFFFFFF, m_mesh.material->diffuseMap.GetAddressOf(), &desc);

		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		m_textureSize = {
			static_cast<float>(desc.Width),
			static_cast<float>(desc.Height)
		};
	}

	if (T_GRAPHICS.isDX12Active)
	{
		m_portalFrame = std::make_unique<PlaneDX12>("Data/Sprites/gear.png", 1.0f, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, 0.0f, 1.0f);
		m_portalFrame->SetShaderDX12(ModelShaderDX12Id::PortalSquare);
	}
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void Teleporter::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	const float radius = m_interractionDistance * scale.x;
	if (player != nullptr && XMFLOAT3LengthSq(player->GetPosition() - (position - DirectX::XMFLOAT3{ 0.0f, 0.5f * scale.y, 0.0f })) < radius * radius)
	{
		m_timer += elapsedTime;
		if (m_timer >= m_portalTime)
		{
			if (m_pWidgetMatching == nullptr)
			{
				m_pWidgetMatching = new WidgetMatching(m_pOnlineController, this);
				UI.Register(m_pWidgetMatching);
			}
		}
	}
	else
	{
		m_timer = 0.0f;
		if (m_pWidgetMatching != nullptr)
		{
			UI.Remove(m_pWidgetMatching);
			m_pOnlineController->EndMatching();
			m_pWidgetMatching = nullptr;
		}
	}

	ModelObject::Update(elapsedTime);

	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	for (int i = 0; i < m_mesh.vertices.size(); i++)
	{
		ModelResource::Vertex& vertice = m_mesh.vertices[i];
		DirectX::XMStoreFloat3(&vertice.position, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_defaultVertices[i].position), Transform));
	}

	if (T_GRAPHICS.isDX12Active)
	{
		m_portalFrame->SetPosition(position);
		m_portalFrame->Update(elapsedTime);
	}
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ参照
	@return		なし
*//***************************************************************************/
void Teleporter::Render(const RenderContext& rc)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	HRESULT hr = rc.deviceContext->Map(m_mesh.vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	ModelResource::Vertex* v = static_cast<ModelResource::Vertex*>(mappedSubresource.pData);
	for (int i = 0; i < 4; i++)
	{
		v[i].position = m_mesh.vertices[i].position;
		v[i].texcoord = m_mesh.vertices[i].texcoord;
	}

	// 頂点バッファの内容の編集を終了する
	rc.deviceContext->Unmap(m_mesh.vertexBuffer.Get(), 0);

	ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
	shader->Begin(rc);
	shader->Draw(rc, m_mesh);
	shader->End(rc);
}

/**************************************************************************//**
	@brief		描画処理
	@param[in]	rc	レンダーコンテンツ参照
	@return		なし
*//***************************************************************************/
void Teleporter::RenderDX12(const RenderContextDX12& rc)
{
	if (m_isVisible)
	{
		m_portalFrame->RenderDX12(rc);
	}
}

/**************************************************************************//**
	@brief		転送開始
	@param[in]	なし
	@return		なし
*//***************************************************************************/
void Teleporter::Teleport()
{
	StageManager::Instance().ChangeStage(m_pStage);
	UI.Remove(m_pWidgetMatching);
	m_pWidgetMatching = nullptr;
	m_pStage = nullptr;
	m_timer = -10.0f;
}

/**************************************************************************//**
	@brief		デストラクタ
	@param[in]	なし
	@return		なし
*//***************************************************************************/
Teleporter::~Teleporter()
{
	if (m_pStage != nullptr)
	{
		delete m_pStage;
		m_pStage = nullptr;
	}

	if (m_mesh.material != nullptr)
		delete m_mesh.material;
	m_mesh.material = nullptr;
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
	@return		なし
*//***************************************************************************/
void TeleportToOpenworld::Update(float elapsedTime)
{
	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	const float radius = 0.5f * scale.x;
	if (player != nullptr && XMFLOAT3LengthSq(player->GetPosition() - position) < radius * radius)
	{
		m_timer += elapsedTime;
		if (m_timer >= m_portalTime)
		{
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame_E4C));
		}
	}

	ModelObject::Update(elapsedTime);
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
	for (int i = 0; i < m_mesh.vertices.size(); i++)
	{
		ModelResource::Vertex& vertice = m_mesh.vertices[i];
		DirectX::XMStoreFloat3(&vertice.position, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_defaultVertices[i].position), Transform));
	}
}