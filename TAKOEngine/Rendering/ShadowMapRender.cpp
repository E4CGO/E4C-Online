//! @file ShadowMapRender.cpp
//! @note

#include <d3d11.h>
#include <imgui.h>
#include "Misc.h"

#include "TAKOEngine\Rendering\Graphics.h"
#include "TAKOEngine\Editor\Camera\CameraManager.h"

#include "TAKOEngine\Rendering\ShadowMapRender.h"

namespace myRenderer
{
	namespace shadow
	{
		//*****************************************************************
		// @brief      初期化
		// @param[in]  なし
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::Initialize()
		{
			for (int i = 0; i < NUM_SHADOW_MAP; i++)
			{
				depthStencil[i].release();
				depthStencil[i] = std::make_unique<DepthStencil>(width, width);
			}
		}

		//*****************************************************************
		// @brief      シャドウマップに描画するモデルを登録
		// @param[in]  model  描画対象のモデルデータを指すポインタ
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::ModelRegister(iModel* model)
		{
			// モデルが既に登録されているか調べる
			std::vector<iModel*>::iterator it = std::find(m_models.begin(), m_models.end(), model);
			if (it != m_models.end())
				return;

			m_models.emplace_back(model);
		}

		//*****************************************************************
		// @brief      シャドウマップに使用するライトを取得
		// @param[in]  light シャドウマップ用に設定する光源を指すポインタ
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::SetShadowLight(Light* light)
		{
			this->light = light;
		}

		//*****************************************************************
		// @brief      登録済みのライトを全削除
		// @param[in]  なし
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::Clear()
		{
			m_models.clear();
			light = nullptr;
		}

		//*****************************************************************
		// @brief      描画
		// @param[in]  なし
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::Render()
		{
			if (!light) return;

			Graphics& graphics = Graphics::Instance();
			ID3D11DeviceContext* dc = graphics.GetDeviceContext();

			// 現在設定されているバッファを退避して初期化して
			UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
			D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;
			{
				dc->RSGetViewports(&cachedViewportCount, cachedViewports);
				dc->OMGetRenderTargets(
					1,
					cachedRenderTargetView.ReleaseAndGetAddressOf(),
					cachedDepthStencilView.ReleaseAndGetAddressOf());
			}

			//シャドウマップの分割エリア定義
			float splitAreaTable[] =
			{
				CameraManager::Instance().GetCamera()->GetNearZ(),
				50.0f,
				150.0f,
				300.0f,
				500.0f,
				CameraManager::Instance().GetCamera()->GetFarZ(),
			};

			//カメラパラメータを取得
			DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&CameraManager::Instance().GetCamera()->GetFront()));
			DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&CameraManager::Instance().GetCamera()->GetRight()));
			DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&CameraManager::Instance().GetCamera()->GetUp()));
			DirectX::XMVECTOR cameraPos = DirectX::XMLoadFloat3(&CameraManager::Instance().GetCamera()->GetEye());

			//ライトビュープロジェクション行列の作成
			DirectX::XMMATRIX viewMatrix, projectionMatrix, viewProjectionMatrix;
			{
				//平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
				DirectX::XMFLOAT3 lightDirection = light->GetDirection();
				DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&lightDirection);
				LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
				viewMatrix = DirectX::XMMatrixLookAtLH(LightPosition, DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

				//シャドウマップに描画したい範囲の射影行列を生成
				projectionMatrix = DirectX::XMMatrixOrthographicLH(10000, 10000, 1.0f, 1000.0f);
				viewProjectionMatrix = viewMatrix * projectionMatrix;
			}

			//描画処理
			//エリアの近平面までの距離
			RenderContext rc;
			for (int i = 0; i < NUM_SHADOW_MAP; ++i)
			{
				// エリアの近平面までの距離
				float nearDepth = splitAreaTable[i + 0];

				// エリアの遠平面までの距離
				float farDepth = splitAreaTable[i + 1];

				//depthClear&RenderTarget設定&VirePort設定
				ID3D11RenderTargetView* rtv = nullptr;
				ID3D11DepthStencilView* dsv = depthStencil[i]->GetDepthStencilView().Get();

				// ここでリソースをアンバインド
				ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
				dc->PSSetShaderResources(10 + i, 1, nullSRV);

				dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				dc->OMSetRenderTargets(0, &rtv, dsv);

				D3D11_VIEWPORT viewport = {};
				viewport.Width = static_cast<float>(depthStencil[i]->GetWidth());
				viewport.Height = static_cast<float>(depthStencil[i]->GetHeight());
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				dc->RSSetViewports(1, &viewport);

				//エリアのを内包する視錐台の8頂点を算出する
				DirectX::XMVECTOR vertex[8];
				{
					//エリアの近平面の中心からの上面までの距離を求める
					float nearY = tanf(CameraManager::Instance().GetCamera()->GetFovY() / 2.0f) * nearDepth;

					//エリアの近平面の中心からの右面までの距離を求める
					float nearX = nearY * CameraManager::Instance().GetCamera()->GetAspect();

					//エリアの遠平面の中心からの上面までの距離を求める
					float farY = tanf(CameraManager::Instance().GetCamera()->GetFovY() / 2.0f) * farDepth;

					//エリアの遠平面の中心からの右面までの距離を求める
					float farX = farY * CameraManager::Instance().GetCamera()->GetAspect();

					//エリアの近平面の中心座標を求める
					DirectX::XMVECTOR nearPosition = DirectX::XMVectorAdd(cameraPos, DirectX::XMVectorScale(cameraFront, nearDepth));

					//エリアの遠平面の中心座標を求める
					DirectX::XMVECTOR farPosition = DirectX::XMVectorAdd(cameraPos, DirectX::XMVectorScale(cameraFront, farDepth));

					//8頂点を求める
					{
						//近平面の右上
						vertex[0] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, nearY), DirectX::XMVectorScale(cameraRight, nearX)));

						//近平面の左上
						vertex[1] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, nearY), DirectX::XMVectorScale(cameraRight, -nearX)));

						//近平面の右下
						vertex[2] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -nearY), DirectX::XMVectorScale(cameraRight, nearX)));

						//近平面の左下
						vertex[3] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -nearY), DirectX::XMVectorScale(cameraRight, -nearX)));

						//遠平面の右上
						vertex[4] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, farY), DirectX::XMVectorScale(cameraRight, farX)));

						//遠平面の左上
						vertex[5] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, farY), DirectX::XMVectorScale(cameraRight, -farX)));

						//遠平面の右下
						vertex[6] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -farY), DirectX::XMVectorScale(cameraRight, farX)));

						//遠平面の左下
						vertex[7] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -farY), DirectX::XMVectorScale(cameraRight, -farX)));
					}
				}

				//8頂点をライトビュープロジェクション空間にして、最大値、最小値を求める
				DirectX::XMFLOAT3 vertexMin(FLT_MAX, FLT_MAX, FLT_MAX), vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
				for (auto& it : vertex)
				{
					DirectX::XMFLOAT3 p;
					DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(it, viewProjectionMatrix));
					vertexMin.x = min(p.x, vertexMin.x);
					vertexMin.y = min(p.y, vertexMin.y);
					vertexMin.z = min(p.z, vertexMin.z);
					vertexMax.x = max(p.x, vertexMax.x);
					vertexMax.y = max(p.y, vertexMax.y);
					vertexMax.z = max(p.z, vertexMax.z);
				}

				//クロップ行列を求める
				float xScale = 2.0f / (vertexMax.x - vertexMin.x);
				float yScale = 2.0f / (vertexMax.y - vertexMin.y);
				float xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
				float yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;
				DirectX::XMMATRIX clopMatrix = DirectX::XMMatrixSet
				(
					xScale, 0, 0, 0,
					0, yScale, 0, 0,
					0, 0, 1, 0,
					xOffset, yOffset, 0, 1
				);

				// ライトビュープロジェクション行列にクロップ行列を乗算
				DirectX::XMStoreFloat4x4(&lightViewProjection[i], viewProjectionMatrix * clopMatrix);

				rc.camera = CameraManager::Instance().GetCamera();
				rc.deviceContext = dc;
				rc.renderState = graphics.GetRenderState();
				DirectX::XMStoreFloat4x4(&view, viewMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.view, viewMatrix);
				DirectX::XMStoreFloat4x4(&projection, projectionMatrix * clopMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.projection, projectionMatrix * clopMatrix);

				//3Dモデル描画
				{
					ModelShader* shader = graphics.GetModelShader(ModelShaderId::ShadowMap);
					shader->Begin(rc);
					for (auto& model : m_models)
					{
						shader->Draw(rc, model);
					}
					shader->End(rc);
				}
			}

			//	元のバッファに戻す
			{
				dc->RSSetViewports(cachedViewportCount, cachedViewports);
				dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
			}
		}

		//*****************************************************************
		// @brief      GUI描画
		// @param[in]  なし
		// @return     なし
		//*****************************************************************
		void ShadowMapRender::DrawDebugGUI()
		{
			//シャドウマップ
			if (ImGui::TreeNode("Shadowmap"))
			{
				ImGui::Text("texture");
				for (int i = 0; i < NUM_SHADOW_MAP; ++i)
				{
					ImGui::Image(depthStencil[i]->GetShaderResourceView().Get(), { 256,256 }, { 0,0 }, { 1,1 }, { 1,1,1,1 });
					ImGui::SameLine();
				}
				ImGui::TreePop();
			}
		}

		//*****************************************************************
		// @brief      シャドウマップ情報をRenderContextに積む
		// @param[in]  なし
		// @return     ShadowMapData
		//*****************************************************************
		ShadowMapData ShadowMapRender::GetShadowMapData()
		{
			ShadowMapData shadowMapData;
			shadowMapData.view = view;
			shadowMapData.projection = projection;
			for (int i = 0; i < NUM_SHADOW_MAP; ++i)
			{
				shadowMapData.shadowMap[i] = depthStencil[i]->GetShaderResourceView().Get();
				shadowMapData.lightViewProjection[i] = lightViewProjection[i];
			}
			return shadowMapData;
		}
	}
}

//*****************************************************************
// @brief      コンストラクタ
// @param[in]  width 幅
// @param[in]  height 高さ
// @return     なし
//*****************************************************************
DepthStencil::DepthStencil(UINT width, UINT height)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	texture2dDesc = {};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
	HRESULT hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	ZeroMemory(&shader_resource_view_desc, sizeof(shader_resource_view_desc));
	shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shaderResourceView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(texture2d.Get(), &depth_stencil_view_desc, depthStencilView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}

//***********************************************************
// @brief       コンストラクタ
// @param[in]   device　    ID3D12Device*
// @return      なし
//***********************************************************
ShadowMapRenderDX12::ShadowMapRenderDX12()
{
}

//***********************************************************
// @brief       初期化
// @param[in]   device　    ID3D12Device*
// @return      なし
//***********************************************************
void ShadowMapRenderDX12::Init(ID3D12Device* device)
{
	CreateFrameBuffer(device);
}

//***********************************************************
// @brief       終了処理
// @param[in]   なし
// @return      なし
//***********************************************************
void ShadowMapRenderDX12::Finalize()
{
	if (m_models.size() != 0)
	{
		m_models.clear();
	}

	if (dsv_descriptor != nullptr)
	{
		Graphics::Instance().GetDepthStencilDescriptorHeap()->PushDescriptor(dsv_descriptor);
	}

	if (srv_descriptor != nullptr)
	{
		Graphics::Instance().GetShaderResourceDescriptorHeap()->PushDescriptor(srv_descriptor);
	}

	if (sampler_descriptor != nullptr)
	{
		Graphics::Instance().GetSamplerDescriptorHeap()->PushDescriptor(sampler_descriptor);
	}
}

//***********************************************************
// @brief       モデル登録
// @param[in]   model　    iModel*
// @return      なし
//***********************************************************
void ShadowMapRenderDX12::ModelRegister(iModel* model)
{
	// モデルが既に登録されているか調べる
	std::vector<iModel*>::iterator it = std::find(m_models.begin(), m_models.end(), model);
	if (it != m_models.end())
		return;

	m_models.emplace_back(model);
}

//***********************************************************
// @brief       描画
// @param[in]   frameBuffer    FrameBufferManager*
// @return      なし
//***********************************************************
void ShadowMapRenderDX12::Render(FrameBufferManager* frameBuffer)
{
	Graphics& graphics = Graphics::Instance();

	// シーン用定数バッファ更新
	const Descriptor* scene_cbv_descriptor = graphics.UpdateSceneConstantBuffer(
		CameraManager::Instance().GetCamera(), 0, 0);

	RenderContextDX12 rc;
	rc.d3d_command_list = frameBuffer->GetCommandList();
	rc.scene_cbv_descriptor = scene_cbv_descriptor;

	// 描画
	{
		// レンダーターゲットを変更
		frameBuffer->SetRenderTarget(dsv_descriptor->GetCpuHandle(), ShadowMapSize);
		frameBuffer->ClearDepthStencilView(dsv_descriptor->GetCpuHandle());

		ModelShaderDX12* shader = graphics.GetModelShaderDX12(ModelShaderDX12Id::shadowMap);
		for (auto& model : m_models)
		{
			// スキニング
			graphics.GetSkinningPipeline()->Compute(rc, model);

			for (const ModelDX12::Mesh& mesh : model->GetMeshes())
			{
				shader->Render(rc, mesh);
			}
		}
	}

	// レンダーターゲットを元に戻す
	frameBuffer->SetRenderTarget(graphics.GetFrameBufferDX12(FrameBufferDX12Id::Scene));
}

//********************************************************
// @brief     フレームバッファ用のコンスタントバッファを作成
// @param[in] なし
// @return    なし
//********************************************************
void ShadowMapRenderDX12::CreateFrameBuffer(ID3D12Device* device)
{
	// ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES d3d_heap_props{};
	d3d_heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_heap_props.CreationNodeMask = 1;
	d3d_heap_props.VisibleNodeMask = 1;

	// リソースの設定
	D3D12_RESOURCE_DESC d3d_resource_desc{};
	d3d_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_resource_desc.Alignment = 0;
	d3d_resource_desc.Width = ShadowMapSize;
	d3d_resource_desc.Height = ShadowMapSize;
	d3d_resource_desc.DepthOrArraySize = 1;
	d3d_resource_desc.MipLevels = 1;
	d3d_resource_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	d3d_resource_desc.SampleDesc.Count = 1;
	d3d_resource_desc.SampleDesc.Quality = 0;
	d3d_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	// リソースの生成
	HRESULT hr = device->CreateCommittedResource(
		&d3d_heap_props,
		D3D12_HEAP_FLAG_NONE,
		&d3d_resource_desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(d3d_dsv_srv_resource.GetAddressOf()));
	d3d_dsv_srv_resource->SetName(L"ShadowMap");

	// ディスクリプタ取得
	dsv_descriptor = Graphics::Instance().GetDepthStencilDescriptorHeap()->PopDescriptor();

	// デプスステンシルビューの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC d3d_dsv_desc{};
	d3d_dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	d3d_dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3d_dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

	// デプスステンシルビュー生成
	device->CreateDepthStencilView(
		d3d_dsv_srv_resource.Get(),
		&d3d_dsv_desc,
		dsv_descriptor->GetCpuHandle());

	// シェーダーリソースビューの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC d3d_srv_desc = {};
	d3d_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3d_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3d_srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
	d3d_srv_desc.Texture2D.MipLevels = d3d_resource_desc.MipLevels;
	d3d_srv_desc.Texture2D.MostDetailedMip = 0;

	// ディスクリプタ取得
	srv_descriptor = Graphics::Instance().GetShaderResourceDescriptorHeap()->PopDescriptor();

	// シェーダリソースビューを生成.
	device->CreateShaderResourceView(
		d3d_dsv_srv_resource.Get(),
		&d3d_srv_desc,
		srv_descriptor->GetCpuHandle());

	// シャドウサンプラー
	{
		// サンプラの設定
		D3D12_SAMPLER_DESC d3d_sampler_desc{};
		d3d_sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		d3d_sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		d3d_sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		d3d_sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		d3d_sampler_desc.MipLODBias = 0;
		d3d_sampler_desc.MaxAnisotropy = 1;
		d3d_sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
		d3d_sampler_desc.BorderColor[0] = D3D12_FLOAT32_MAX;
		d3d_sampler_desc.BorderColor[1] = D3D12_FLOAT32_MAX;
		d3d_sampler_desc.BorderColor[2] = D3D12_FLOAT32_MAX;
		d3d_sampler_desc.BorderColor[3] = D3D12_FLOAT32_MAX;
		d3d_sampler_desc.MinLOD = 0;
		d3d_sampler_desc.MaxLOD = 0;

		// ディスクリプタ取得
		sampler_descriptor = Graphics::Instance().GetSamplerDescriptorHeap()->PopDescriptor();

		// サンプラー生成
		device->CreateSampler(
			&d3d_sampler_desc,
			sampler_descriptor->GetCpuHandle());
	}
}