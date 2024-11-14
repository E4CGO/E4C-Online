//! @file Teleporter.h
//! @note 

#ifndef __INCLUDE_TELEPORTER_H__
#define __INCLUDE_TELEPORTER_H__

#include "GameObject/ModelObject.h"
#include "Scene/Stage/Stage.h"

/**************************************************************************//**
	@class	Teleporter
	@brief	ステージ転送用ゲームオブジェクトクラス
	@par	[説明]
*//***************************************************************************/
class Teleporter : public ModelObject
{
public:
	Teleporter(Stage* stage);
	~Teleporter();

	virtual void Update(float elapsedTime) override;
	void Render(const RenderContext& rc) override;
private:
	Stage* m_pStage;
	ModelResource::Mesh m_mesh;
	
	float m_portalTime = 3.0f;
	float m_timer = 0.0f;
	float m_radius = 0.5f;

	DirectX::XMFLOAT2 m_textureSize = {};

	const std::vector<ModelResource::Vertex> m_defaultVertices = {
			{ { -0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 0.0f } },
			{ { +0.5, +0.5f, 0.0f }, {}, {}, { 0.0f, 1.0f } },
			{ { -0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 0.0f } },
			{ { +0.5, -0.5f, 0.0f }, {}, {}, { 1.0f, 1.0f } },
	};
};
#endif // !__INCLUDE_TELEPORTER_H__
