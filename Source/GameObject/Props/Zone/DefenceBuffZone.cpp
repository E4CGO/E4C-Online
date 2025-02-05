//! @file DefenceBuffZone.cpp
//! @note 

#include "DefenceBuffZone.h"
#include "GameObject/Character/Player/PlayerCharacterManager.h"

/**************************************************************************//**
	@brief		コンストラクタ
	@param[in]	owner オーナー
*//***************************************************************************/
DefenceBuffZone::DefenceBuffZone(PlayerCharacter* owner) : ZoneObject(owner, 10.0f, 0.0f), m_pOwner(owner)
{
	m_effect = std::make_unique<ModelObject>("Data/Model/Object/EffectZone.glb", 1.0f, ModelObject::RENDER_MODE::DX12, ModelObject::MODEL_TYPE::LHS_PBR);
	m_effect->SetShader("EffectZone", ModelShaderDX12Id::Zone);
	m_effect->SetScale({ 0.0f, 0.0f, 0.0f });

	m_ScaleTimer = 0.0f;
}

/**************************************************************************//**
	@brief		更新処理
	@param[in]	elapsedTime	経過時間
*//***************************************************************************/
void DefenceBuffZone::Update(float elapsedTime)
{
	ZoneObject::Update(elapsedTime);
	if (m_timer <= 0.0f) return;

	m_ScaleTimer += elapsedTime;
	// ゾーン生存
	float zoneRadio = m_ScaleTimer / m_ScaleTime;
	if (zoneRadio > 1.0f) zoneRadio = 1.0f; // 一を超えない
	m_radius = m_maxZoneRadius * zoneRadio;
	this->SetPosition(m_pOwner->GetPosition());

	m_effect->SetPosition(position);

	PlayerCharacter* player = PlayerCharacterManager::Instance().GetPlayerCharacterById();
	if (XMFLOAT3HorizontalLengthSq((player->GetPosition() - position)) <= m_radius * m_radius)
	{
		player->SetDefenceBuff();
	}

	m_effect->SetScale({ m_radius , 1.0f , m_radius });
	m_effect->Update(elapsedTime);
}

/**************************************************************************//**
	@brief    DX11の描画
	@param[in]    rc
*//***************************************************************************/
void DefenceBuffZone::Render(const RenderContext& rc)
{
	m_effect->Render(rc);
}

/**************************************************************************//**
	@brief    DX11の描画
	@param[in]    rc
*//***************************************************************************/
void DefenceBuffZone::RenderDX12(const RenderContextDX12& rc)
{
	m_effect->RenderDX12(rc);
}
