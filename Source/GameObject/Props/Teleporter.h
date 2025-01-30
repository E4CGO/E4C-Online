//! @file Teleporter.h
//! @note

#ifndef __INCLUDE_TELEPORTER_H__
#define __INCLUDE_TELEPORTER_H__

#include "GameObject/ModelObject.h"
#include "TAKOEngine/Rendering/Plane.h"
#include "Scene/Stage/Stage.h"
#include "UI/Widget/WidgetMatching.h"
#include "Network/OnlineController.h"

/**************************************************************************//**
	@class	Teleporter
	@brief	ステージ転送用ゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class Teleporter : public ModelObject
{
public:
	Teleporter(Stage* stage, Online::OnlineController* onlineController);
	~Teleporter();

	virtual void Update(float elapsedTime) override;
	virtual void Render(const RenderContext& rc) override;
	virtual void RenderDX12(const RenderContextDX12& rc) override;

	void SetVisibility(bool isVisible) { this->m_isVisible = isVisible; }

	void SetInteractionDistance(float newDist) { this->m_interactionDistance = newDist; }

	virtual void Teleport();

	Stage* GetStage() { return m_pStage; }
protected:
	Stage* m_pStage;

	ModelResource::Mesh m_mesh;

	float m_portalTime = 3.0f;
	float m_timer = 0.0f;
	float m_interactionDistance = 5.0f;

	DirectX::XMFLOAT2 m_textureSize = {};

	const std::vector<ModelResource::Vertex> m_defaultVertices = {
		{ { -0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 0.0f } },
		{ { +0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 1.0f } },
		{ { -0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 0.0f } },
		{ { +0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 1.0f } },
	};
	Online::OnlineController* m_pOnlineController;

	WidgetMatching* m_pWidgetMatching = nullptr;

	std::unique_ptr<PlaneDX12> m_portalFrame;

	bool m_isVisible = false;
};

class TeleportToOpenworld : public Teleporter
{
public:
	TeleportToOpenworld() : Teleporter(nullptr, nullptr) {}
	virtual void Update(float elapsedTime) override;
};
#endif // !__INCLUDE_TELEPORTER_H__
