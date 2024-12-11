#include "Cylinder.h"
#include "Misc.h"
#include "GpuResourceUtils.h"
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/Shaders/PlaneShader.h"

// コンストラクタ
Cylinder::Cylinder(ID3D11Device* device, const char* filename, float scaling)
	: ModelObject(filename, scaling, ModelObject::RENDER_MODE::NOMODEL)
{
	HRESULT hr = S_OK;
}

void Cylinder::Update(float elapsedTime)
{
	// スケール行列生成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列生成
	DirectX::XMMATRIX R = AnglesToMatrix(angle);
	// 位置行列生成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

void Cylinder::Render(const RenderContext& rc)
{
	ModelShader* shader = T_GRAPHICS.GetModelShader(m_shaderId);
	shader->Begin(rc);
	shader->Draw(rc, mesh);
	shader->End(rc);
}