#include <d3d11.h>
#include <imgui.h>
#include "Misc.h"

#include "TAKOEngine\Rendering\ShadowMapRender.h"
#include "TAKOEngine\Rendering\Graphics.h"

namespace myRenderer
{
	namespace shadow
	{
		// 初期化
		void ShadowMapRender::Initialize()
		{
			for (int i = 0; i < NUM_SHADOW_MAP; i++)
			{
				depthStencil[i].release();
				depthStencil[i] = std::make_unique<DepthStencil>(width, width);
			}
		}

		//シャドウマップに描画するモデルを登録
		void ShadowMapRender::ModelRegister(Model* model)
		{
			// モデルが既に登録されているか調べる
			std::vector<Model*>::iterator it = std::find(m_models.begin(), m_models.end(), model);
			if (it != m_models.end())
				return;

			m_models.emplace_back(model);
		}

		//シャドウマップに使用するライトを取得
		void ShadowMapRender::SetShadowLight(Light* light)
		{
			this->light = light;
		}

		//登録済みのライトを全削除
		void ShadowMapRender::Clear()
		{
			for (Model* model : m_models)
			{
				delete model;
			}
			m_models.clear();
			light = nullptr;
		}

		//描画
		void ShadowMapRender::Render()
		{
			if (!light) return;

			Graphics& graphics      = Graphics::Instance();
			ID3D11DeviceContext* dc = graphics.GetDeviceContext();
			Camera& camera          = Camera::Instance();

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
				Camera::Instance().GetNearZ(),
				50.0f,
				150.0f,
				300.0f,
				500.0f,
				Camera::Instance().GetFarZ(),
			};

			//カメラパラメータを取得
			DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetFront()));
			DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetRight()));
			DirectX::XMVECTOR cameraUp    = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetUp()));
			DirectX::XMVECTOR cameraPos   = DirectX::XMLoadFloat3(&camera.GetEye());

			//ライトビュープロジェクション行列の作成
			DirectX::XMMATRIX viewMatrix, projectionMatrix, viewProjectionMatrix;
			{
				//平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
				DirectX::XMFLOAT3 lightDirection = light->GetDirection();
				DirectX::XMVECTOR LightPosition  = DirectX::XMLoadFloat3(&lightDirection);
				LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
				viewMatrix    = DirectX::XMMatrixLookAtLH(LightPosition, DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

				//シャドウマップに描画したい範囲の射影行列を生成
				projectionMatrix     = DirectX::XMMatrixOrthographicLH(10000, 10000, 1.0f, 1000.0f);
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
				viewport.Width    = static_cast<float>(depthStencil[i]->GetWidth());
				viewport.Height   = static_cast<float>(depthStencil[i]->GetHeight());
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				dc->RSSetViewports(1, &viewport);

				//エリアのを内包する視錐台の8頂点を算出する
				DirectX::XMVECTOR vertex[8];
				{
					//エリアの近平面の中心からの上面までの距離を求める
					float nearY = tanf(camera.GetFovY() / 2.0f) * nearDepth;

					//エリアの近平面の中心からの右面までの距離を求める
					float nearX = nearY * camera.GetAspect();

					//エリアの遠平面の中心からの上面までの距離を求める
					float farY = tanf(camera.GetFovY() / 2.0f) * farDepth;

					//エリアの遠平面の中心からの右面までの距離を求める
					float farX = farY * camera.GetAspect();

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

				rc.camera = &camera;
				rc.deviceContext = dc;
				rc.renderState   = graphics.GetRenderState();
				DirectX::XMStoreFloat4x4(&view, viewMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.view, viewMatrix);
				DirectX::XMStoreFloat4x4(&projection, projectionMatrix* clopMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.projection, projectionMatrix* clopMatrix);

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

		void ShadowMapRender::DrawDebugGUI()
		{
			//シャドウマップ
			if (ImGui::TreeNode("Shadowmap"))
			{
				ImGui::Text("texture");
				for (int i = 0; i < NUM_SHADOW_MAP; ++i)
				{
					ImGui::Image(depthStencil[i]->GetShaderResourceView().Get(), {256,256}, {0,0}, {1,1}, {1,1,1,1});
					ImGui::SameLine();
				}
				ImGui::TreePop();
			}
		}

		// シャドウマップ情報をRenderContextに積む
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

DepthStencil::DepthStencil(UINT width, UINT height)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

	texture2dDesc = {};
	texture2dDesc.Width              = width;
	texture2dDesc.Height             = height;
	texture2dDesc.MipLevels          = 1;
	texture2dDesc.ArraySize          = 1;
	texture2dDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count   = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage              = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags     = 0;
	texture2dDesc.MiscFlags          = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
	HRESULT hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	ZeroMemory(&shader_resource_view_desc, sizeof(shader_resource_view_desc));
	shader_resource_view_desc.Format                    = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shader_resource_view_desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
	shader_resource_view_desc.Texture2D.MipLevels       = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shaderResourceView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
	depth_stencil_view_desc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags         = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(texture2d.Get(), &depth_stencil_view_desc, depthStencilView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
