#include <d3d11.h>
#include <imgui.h>
#include "Misc.h"

#include "TAKOEngine\Rendering\ShadowMapRender.h"
#include "TAKOEngine\Rendering\Graphics.h"

namespace myRenderer
{
	namespace shadow
	{
		// ������
		void ShadowMapRender::Initialize()
		{
			for (int i = 0; i < NUM_SHADOW_MAP; i++)
			{
				depthStencil[i].release();
				depthStencil[i] = std::make_unique<DepthStencil>(width, width);
			}
		}

		//�V���h�E�}�b�v�ɕ`�悷�郂�f����o�^
		void ShadowMapRender::ModelRegister(Model* model)
		{
			// ���f�������ɓo�^����Ă��邩���ׂ�
			std::vector<Model*>::iterator it = std::find(m_models.begin(), m_models.end(), model);
			if (it != m_models.end())
				return;

			m_models.emplace_back(model);
		}

		//�V���h�E�}�b�v�Ɏg�p���郉�C�g���擾
		void ShadowMapRender::SetShadowLight(Light* light)
		{
			this->light = light;
		}

		//�o�^�ς݂̃��C�g��S�폜
		void ShadowMapRender::Clear()
		{
			m_models.clear();
			light = nullptr;
		}

		//�`��
		void ShadowMapRender::Render()
		{
			if (!light) return;

			Graphics& graphics      = Graphics::Instance();
			ID3D11DeviceContext* dc = graphics.GetDeviceContext();
			Camera& camera          = Camera::Instance();

			// ���ݐݒ肳��Ă���o�b�t�@��ޔ����ď���������
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

			//�V���h�E�}�b�v�̕����G���A��`
			float splitAreaTable[] =
			{
				Camera::Instance().GetNearZ(),
				50.0f,
				150.0f,
				300.0f,
				500.0f,
				Camera::Instance().GetFarZ(),
			};

			//�J�����p�����[�^���擾
			DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetFront()));
			DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetRight()));
			DirectX::XMVECTOR cameraUp    = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetUp()));
			DirectX::XMVECTOR cameraPos   = DirectX::XMLoadFloat3(&camera.GetEye());

			//���C�g�r���[�v���W�F�N�V�����s��̍쐬
			DirectX::XMMATRIX viewMatrix, projectionMatrix, viewProjectionMatrix;
			{
				//���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s��𐶐�
				DirectX::XMFLOAT3 lightDirection = light->GetDirection();
				DirectX::XMVECTOR LightPosition  = DirectX::XMLoadFloat3(&lightDirection);
				LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
				viewMatrix    = DirectX::XMMatrixLookAtLH(LightPosition, DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

				//�V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
				projectionMatrix     = DirectX::XMMatrixOrthographicLH(10000, 10000, 1.0f, 1000.0f);
				viewProjectionMatrix = viewMatrix * projectionMatrix;
			}

			//�`�揈��
			//�G���A�̋ߕ��ʂ܂ł̋���
			RenderContext rc;
			for (int i = 0; i < NUM_SHADOW_MAP; ++i)
			{
				// �G���A�̋ߕ��ʂ܂ł̋���
				float nearDepth = splitAreaTable[i + 0];

				// �G���A�̉����ʂ܂ł̋���
				float farDepth = splitAreaTable[i + 1];

				//depthClear&RenderTarget�ݒ�&VirePort�ݒ�
				ID3D11RenderTargetView* rtv = nullptr;
				ID3D11DepthStencilView* dsv = depthStencil[i]->GetDepthStencilView().Get();
				
				// �����Ń��\�[�X���A���o�C���h
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

				//�G���A�̂����鎋�����8���_���Z�o����
				DirectX::XMVECTOR vertex[8];
				{
					//�G���A�̋ߕ��ʂ̒��S����̏�ʂ܂ł̋��������߂�
					float nearY = tanf(camera.GetFovY() / 2.0f) * nearDepth;

					//�G���A�̋ߕ��ʂ̒��S����̉E�ʂ܂ł̋��������߂�
					float nearX = nearY * camera.GetAspect();

					//�G���A�̉����ʂ̒��S����̏�ʂ܂ł̋��������߂�
					float farY = tanf(camera.GetFovY() / 2.0f) * farDepth;

					//�G���A�̉����ʂ̒��S����̉E�ʂ܂ł̋��������߂�
					float farX = farY * camera.GetAspect();

					//�G���A�̋ߕ��ʂ̒��S���W�����߂�
					DirectX::XMVECTOR nearPosition = DirectX::XMVectorAdd(cameraPos, DirectX::XMVectorScale(cameraFront, nearDepth));

					//�G���A�̉����ʂ̒��S���W�����߂�
					DirectX::XMVECTOR farPosition = DirectX::XMVectorAdd(cameraPos, DirectX::XMVectorScale(cameraFront, farDepth));

					//8���_�����߂�
					{
						//�ߕ��ʂ̉E��
						vertex[0] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, nearY), DirectX::XMVectorScale(cameraRight, nearX)));

						//�ߕ��ʂ̍���
						vertex[1] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, nearY), DirectX::XMVectorScale(cameraRight, -nearX)));

						//�ߕ��ʂ̉E��
						vertex[2] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -nearY), DirectX::XMVectorScale(cameraRight, nearX)));

						//�ߕ��ʂ̍���
						vertex[3] = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -nearY), DirectX::XMVectorScale(cameraRight, -nearX)));

						//�����ʂ̉E��
						vertex[4] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, farY), DirectX::XMVectorScale(cameraRight, farX)));

						//�����ʂ̍���
						vertex[5] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, farY), DirectX::XMVectorScale(cameraRight, -farX)));

						//�����ʂ̉E��
						vertex[6] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -farY), DirectX::XMVectorScale(cameraRight, farX)));

						//�����ʂ̍���
						vertex[7] = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorAdd(DirectX::XMVectorScale(cameraUp, -farY), DirectX::XMVectorScale(cameraRight, -farX)));
					}
				}

				//8���_�����C�g�r���[�v���W�F�N�V������Ԃɂ��āA�ő�l�A�ŏ��l�����߂�
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

				//�N���b�v�s������߂�
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

				// ���C�g�r���[�v���W�F�N�V�����s��ɃN���b�v�s�����Z
				DirectX::XMStoreFloat4x4(&lightViewProjection[i], viewProjectionMatrix * clopMatrix);

				rc.camera = &camera;
				rc.deviceContext = dc;
				rc.renderState   = graphics.GetRenderState();
				DirectX::XMStoreFloat4x4(&view, viewMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.view, viewMatrix);
				DirectX::XMStoreFloat4x4(&projection, projectionMatrix* clopMatrix);
				DirectX::XMStoreFloat4x4(&rc.shadowMapData.projection, projectionMatrix* clopMatrix);

				//3D���f���`��
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

			//	���̃o�b�t�@�ɖ߂�
			{
				dc->RSSetViewports(cachedViewportCount, cachedViewports);
				dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
			}
		}

		void ShadowMapRender::DrawDebugGUI()
		{
			//�V���h�E�}�b�v
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

		// �V���h�E�}�b�v����RenderContext�ɐς�
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
