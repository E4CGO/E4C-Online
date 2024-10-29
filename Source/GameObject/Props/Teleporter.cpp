#include "Teleporter.h"

#include "TAKOEngine/Tool/Mathf.h"

float CalculateDistance(const DirectX::XMFLOAT3& point1, const DirectX::XMFLOAT3& point2)
{
	DirectX::XMVECTOR vec1 = DirectX::XMLoadFloat3(&point1);
	DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat3(&point2);

	DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(vec1, vec2);

	DirectX::XMVECTOR length = DirectX::XMVector3Length(difference);

	float distance;
	DirectX::XMStoreFloat(&distance, length);

	return distance;
}

void Teleporter::Update(float elapsedTime)
{
	UpdateModel(elapsedTime);

	if (portalTimer <= 0) SetPortalReady();
}

void Teleporter::UpdateModel(float elapsedTime)
{
	ModelObject::Update(elapsedTime);
}

void Teleporter::Render(const RenderContext& rc)
{
	ModelObject::Render(rc);
}

void Teleporter::CheckPlayer(DirectX::XMFLOAT3 playerCoords, float elapsedTime)
{
	if (CalculateDistance(this->GetPosition(), playerCoords) <= 10)
	{
		portalTimer -= elapsedTime;
		return;
	}
	portalTimer = 3.0f;
}