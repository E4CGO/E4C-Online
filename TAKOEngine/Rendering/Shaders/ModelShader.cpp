//! @file ModelShader.cpp
//! @note

#include <cassert>
#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/GpuResourceUtils.h"
#include "TAKOEngine/Rendering/Shaders/ModelShader.h"
#include "TAKOEngine/Rendering/FrustumCulling.h"

//******************************************************************
// @brief       コンストラクタ
// @param[in]   device ID3D11Device*
// @param[in]   vs     VertexShaderの名前
// @param[in]   ps     PixelShaderの名前
// @return      なし
//******************************************************************
ModelShader::ModelShader(ID3D11Device* device, const char* vs, const char* ps)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 頂点シェーダー
	GpuResourceUtils::LoadVertexShader(
		device,
		vs,
		inputElementDesc,
		_countof(inputElementDesc),
		inputLayout.GetAddressOf(),
		vertexShader.GetAddressOf());

	// ピクセルシェーダー
	if (ps)
	{
		GpuResourceUtils::LoadPixelShader(
			device,
			ps,
			pixelShader.GetAddressOf());
	}

	// シーン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		sceneConstantBuffer.GetAddressOf());

	// メッシュ用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbMesh),
		meshConstantBuffer.GetAddressOf());

	// スケルトン用定数バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbSkeleton),
		skeletonConstantBuffer.GetAddressOf());

	// シャドウマップ用バッファ
	GpuResourceUtils::CreateConstantBuffer(
		device,
		sizeof(CbShadowMap),
		shadowMapConstantBuffer.GetAddressOf());
}

//******************************************************************
// @brief       描画開始
// @param[in]   rc  レンダーコンテキスト
// @return      なし
//******************************************************************
void ModelShader::Begin(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// シェーダー設定
	dc->IASetInputLayout(inputLayout.Get());
	rc.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	rc.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	// 定数バッファ設定
	ID3D11Buffer* constantBuffers[] =
	{
		sceneConstantBuffer.Get(),
		meshConstantBuffer.Get(),
		skeletonConstantBuffer.Get(),
		shadowMapConstantBuffer.Get(),
	};
	rc.deviceContext->VSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);
	rc.deviceContext->PSSetConstantBuffers(0, _countof(constantBuffers), constantBuffers);

	// サンプラステート
	ID3D11SamplerState* samplerStates[] =
	{
		rc.renderState->GetSamplerState(SamplerState::LinearWrap),
	};
	dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);

	ID3D11SamplerState* shadowSampler = rc.renderState->GetSamplerState(SamplerState::ShadowMap);
	dc->PSSetSamplers(10, 1, &shadowSampler);

	// レンダーステート設定
	SetRenderState(rc);

	// シーン用定数バッファ更新
	CbScene cbScene{};
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.camera->GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.camera->GetProjection());
	DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

	cbScene.directionalLightData = rc.directionalLightData;
	cbScene.ambientLightColor = rc.ambientLightColor;
	memcpy_s(
		cbScene.pointLightData,
		sizeof(cbScene.pointLightData),
		rc.pointLightData,
		sizeof(rc.pointLightData)
	);
	cbScene.pointLightCount = rc.pointLightCount;
	memcpy_s(
		cbScene.spotLightData,
		sizeof(cbScene.spotLightData),
		rc.spotLightData,
		sizeof(rc.spotLightData)
	);
	cbScene.spotLightCount = rc.spotLightCount;

	const DirectX::XMFLOAT3& eye = rc.camera->GetEye();
	cbScene.cameraPosition.x = eye.x;
	cbScene.cameraPosition.y = eye.y;
	cbScene.cameraPosition.z = eye.z;
	dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);

	// シャドウマップ用定数バッファ更新
	CbShadowMap cbShadowMap;
	cbShadowMap.shadowColor = rc.shadowMapData.shadowColor;
	for (int i = 0; i < myRenderer::NUM_SHADOW_MAP; ++i)
	{
		//バイアスの処理が４枚以上は考慮していないのでアサートで止めておく
		assert(myRenderer::NUM_SHADOW_MAP <= 4);
		(&cbShadowMap.shadowBias.x)[i] = rc.shadowMapData.shadowBias[i];
		cbShadowMap.lightViewProjection[i] = rc.shadowMapData.lightViewProjection[i];
	}
	rc.deviceContext->UpdateSubresource(shadowMapConstantBuffer.Get(), 0, 0, &cbShadowMap, 0, 0);

	// シャドウマップ設定
	ID3D11ShaderResourceView* srvs[myRenderer::NUM_SHADOW_MAP];
	for (int i = 0; i < myRenderer::NUM_SHADOW_MAP; i++)
	{
		srvs[i] = rc.shadowMapData.shadowMap[i];
	}
	rc.deviceContext->PSSetShaderResources(10, ARRAYSIZE(srvs), srvs);
}

//******************************************************************
// @brief       描画終了
// @param[in]   rc     レンダーコンテキスト
// @return      なし
//******************************************************************
void ModelShader::End(const RenderContext& rc)
{
}

//******************************************************************
// @brief       モデル描画
// @param[in]   rc     レンダーコンテキスト
// @param[in]   model  描画対象のモデルデータを指すポインタ
// @param[in]   color  マテリアルカラー
// @return      なし
//******************************************************************
void ModelShader::Draw(const RenderContext& rc, const iModel* model, DirectX::XMFLOAT4 color)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	const ModelResource* resource = model->GetResource();
	const std::vector<iModel::Node>& nodes = model->GetNodes();

	// カメラに写っている範囲のオブジェクトをフラグでマークする配列を用意
	std::vector<bool> visibleObjects(model->GetMeshes().size(), false);

	// TODO: visibleObjects.size == 0
	if (visibleObjects.size() == 0) return;

	// 視錐台カリングを実行して可視オブジェクトをマーク
	FrustumCulling::FrustumCullingFlag(CameraManager::Instance().GetCamera(), model->GetMeshes(), visibleObjects);
	int culling = 0;

	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		if (!visibleObjects[culling++]) continue;

		// 頂点バッファ設定
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// メッシュ用定数バッファ更新
		CbMesh cbMesh{};
		::memset(&cbMesh, 0, sizeof(cbMesh));
		cbMesh.materialColor = mesh.material->color;
		cbMesh.materialColor.x *= color.x;
		cbMesh.materialColor.y *= color.y;
		cbMesh.materialColor.z *= color.z;
		cbMesh.materialColor.w *= color.w;
		cbMesh.linearGamma = model->GetLinearGamma();
		cbMesh.shaderData = rc.shaderData;
		dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

		// スケルトン用定数バッファ更新
		CbSkeleton cbSkeleton{};
		if (mesh.bones.size() > 0)
		{
			for (size_t i = 0; i < mesh.bones.size(); ++i)
			{
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.bones.at(i).nodeIndex).worldTransform);
				DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.bones.at(i).offsetTransform);
				DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&cbSkeleton.boneTransforms[i], boneTransform);
			}
		}
		else
		{
			cbSkeleton.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		}
		rc.deviceContext->UpdateSubresource(skeletonConstantBuffer.Get(), 0, 0, &cbSkeleton, 0, 0);

		// シェーダーリソースビュー設定
		SetShaderResourceView(mesh, dc);

		// 描画
		dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
	}
}

void ModelShader::Draw(const RenderContext& rc, const ModelResource::Mesh& mesh)
{
}