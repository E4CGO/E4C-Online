//! @file Teleporter.h
//! @note 

#ifndef __INCLUDE_TELEPORTER_H__
#define __INCLUDE_TELEPORTER_H__

#include "GameObject/ModelObject.h"
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
	void Render(const RenderContext& rc) override;

	void Teleport();

	Stage* GetStage() { return m_pStage; }
protected:
	Stage* m_pStage;
	
	ModelResource::Mesh m_mesh;

	float m_portalTime = 3.0f;
	float m_timer = 0.0f;

	DirectX::XMFLOAT2 m_textureSize = {};

	const std::vector<ModelResource::Vertex> m_defaultVertices = {
		{ { -0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 0.0f } },
		{ { +0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 1.0f } },
		{ { -0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 0.0f } },
		{ { +0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 1.0f } },
	};
	Online::OnlineController* m_pOnlineController;

	WidgetMatching* m_pWidgetMatching = nullptr;

};


class TeleportToOpenworld : public Teleporter
{
public:
	TeleportToOpenworld() : Teleporter(nullptr, nullptr) {}
	virtual void Update(float elapsedTime) override;
};
#endif // !__INCLUDE_TELEPORTER_H__
