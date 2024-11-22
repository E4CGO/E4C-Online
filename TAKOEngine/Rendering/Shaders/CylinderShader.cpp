#include "CylinderShader.h"

CylinderShader::CylinderShader(ID3D11Device* device, const char* vs, const char* ps) : ModelShader()
{
}
void CylinderShader::SetRenderState(const RenderContext& rc)
{
}

void CylinderShader::Begin(const RenderContext& rc)
{
}

void CylinderShader::SetShaderResourceView(const ModelResource::Mesh& mesh, ID3D11DeviceContext*& dc)
{
}

void CylinderShader::Draw(const RenderContext& rc, const ModelResource::Mesh& mesh)
{
}

void CylinderShader::End(const RenderContext& rc)
{
}