//! @file PostprocessingRenderer.h
//! @note 

#include <imgui.h>

#include "TAKOEngine/Runtime/tentacle_lib.h"
#include "PostprocessingRenderer.h"

//******************************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//******************************************************************
PostprocessingRenderer::PostprocessingRenderer()
{
	Graphics& graphics = Graphics::Instance();

	renderSprite = std::make_unique<Sprite>(graphics.GetDevice());

	bloomData.gaussianFilterData.textureSize.x = graphics.GetScreenWidth() / 2.0f;
	bloomData.gaussianFilterData.textureSize.y = graphics.GetScreenHeight() / 2.0f;
}

//******************************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//******************************************************************
PostprocessingRenderer::~PostprocessingRenderer()
{
}

//******************************************************************
// @brief       描画
// @param[in]   deviceContext　デバイスコンテキスト
// @return      なし
//******************************************************************
void PostprocessingRenderer::Render(ID3D11DeviceContext* deviceContext)
{
	// 現在設定されているバッファを退避して初期化して
	UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;
	{
		deviceContext->RSGetViewports(&cachedViewportCount, cachedViewports);
		deviceContext->OMGetRenderTargets(
			1,
			cachedRenderTargetView.ReleaseAndGetAddressOf(),
			cachedDepthStencilView.ReleaseAndGetAddressOf()
		);
	}

	RenderContext	rc;
	rc.deviceContext = deviceContext;
	rc.renderState = T_GRAPHICS.GetRenderState();
	rc.luminanceExtractionData = bloomData.luminanceExtractionData;
	rc.gaussianFilterData = bloomData.gaussianFilterData;

	//	高輝度抽出用バッファに描画先を変更して高輝度抽出
	{
		//	描画先を変更
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

		rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

		//	高輝度抽出処理
		SpriteShader* shader = T_GRAPHICS.GetSpriteShader(SpriteShaderId::LuminanceExtraction);
		shader->Begin(rc);
		//	描画対象を変更
		renderSprite->SetShaderResourceView(
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetShaderResourceView(),
			static_cast<UINT>(T_GRAPHICS.GetScreenWidth()),
			static_cast<UINT>(T_GRAPHICS.GetScreenHeight())
		);
		renderSprite->Update(
			0, 0,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Width,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Height,
			0, 0,
			T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
			0,
			1, 1, 1, 1
		);
		shader->Draw(rc, renderSprite.get());
		shader->End(rc);
	}

	// 抽出した高輝度描画対象を暈して書き込む
	{
		//	描画先を変更
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::GaussianBlur)->Clear(T_GRAPHICS.GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);
		T_GRAPHICS.GetFrameBuffer(FrameBufferId::GaussianBlur)->SetRenderTarget(T_GRAPHICS.GetDeviceContext());

		rc.deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);

		//	高輝度抽出処理
		SpriteShader* shader = T_GRAPHICS.GetSpriteShader(SpriteShaderId::GaussianBlur);
		shader->Begin(rc);

		//	描画対象を変更
		renderSprite->SetShaderResourceView(
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetShaderResourceView(),
			static_cast<UINT>(T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Width),
			static_cast<UINT>(T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Height)
		);
		renderSprite->Update(
			0, 0,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::GaussianBlur)->GetViewport().Width,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::GaussianBlur)->GetViewport().Height,
			0, 0,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Width,
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Luminance)->GetViewport().Height,
			0,
			1, 1, 1, 1
		);
		shader->Draw(rc, renderSprite.get());
		shader->End(rc);
	}

	//	元のバッファに戻す
	{
		deviceContext->RSSetViewports(cachedViewportCount, cachedViewports);
		deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
	}

	// 抽出した高輝度描画対象を書き込む
	{
		//	高輝度抽出処理
		SpriteShader* shader = T_GRAPHICS.GetSpriteShader(SpriteShaderId::Finalpass);
		shader->Begin(rc);

		renderSprite->SetShaderResourceView(
			T_GRAPHICS.GetFrameBuffer(FrameBufferId::Scene)->GetShaderResourceView(),
			static_cast<UINT>(T_GRAPHICS.GetScreenWidth()),
			static_cast<UINT>(T_GRAPHICS.GetScreenHeight())
		);
		renderSprite->Update(
			-1, -1,
			T_GRAPHICS.GetScreenWidth() + 2, T_GRAPHICS.GetScreenHeight() + 2,
			0, 0,
			T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
			0,
			1, 1, 1, 1
		);

		//	シェーダーに渡すテクスチャを設定
		rc.finalpassnData.bloomTexture = T_GRAPHICS.GetFrameBuffer(FrameBufferId::GaussianBlur)->GetShaderResourceView().Get();
		rc.colorGradingData = colorGradingData;

		shader->Draw(rc, renderSprite.get());

		shader->End(rc);
	}
}

//******************************************************************
// @brief       デバッグ情報の表示
// @param[in]   なし
// @return      なし
//******************************************************************
void PostprocessingRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("PostProcess"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
			ImGui::SliderInt("kernelSize", &bloomData.gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
			ImGui::SliderFloat("deviation", &bloomData.gaussianFilterData.deviation, 1.0f, 10.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ColorGrading"))
		{
			ImGui::SliderFloat("hueShift", &colorGradingData.hueShift, 0.0f, 360.0f);
			ImGui::SliderFloat("saturation", &colorGradingData.saturation, 0.0f, 2.0f);
			ImGui::SliderFloat("brightness", &colorGradingData.brightness, 0.0f, 2.0f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

//******************************************************************
// @brief       コンストラクタ
// @param[in]   なし
// @return      なし
//******************************************************************
PostprocessingRendererDX12::PostprocessingRendererDX12()
{
	Graphics& graphics = Graphics::Instance();

	renderSprite[static_cast<int>(RenderTarget::Luminance)] = std::make_unique<SpriteDX12>(1, T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene)->GetRenderTargetTexture());
	renderSprite[static_cast<int>(RenderTarget::GaussianBlur)] = std::make_unique<SpriteDX12>(1, T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance)->GetRenderTargetTexture());
	renderSprite[static_cast<int>(RenderTarget::Finalpass)] = std::make_unique<SpriteDX12>(1, T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Scene)->GetRenderTargetTexture());

	bloomData.gaussianFilterData.textureSize.x = static_cast<float>(renderSprite[static_cast<int>(RenderTarget::GaussianBlur)]->GetTextureWidth());
	bloomData.gaussianFilterData.textureSize.y = static_cast<float>(renderSprite[static_cast<int>(RenderTarget::GaussianBlur)]->GetTextureHeight());
}

//******************************************************************
// @brief       デストラクタ
// @param[in]   なし
// @return      なし
//******************************************************************
PostprocessingRendererDX12::~PostprocessingRendererDX12()
{
}

//******************************************************************
// @brief       描画
// @param[in]   framBuffer　フレームバッファ
// @return      なし
//******************************************************************
void PostprocessingRendererDX12::Render(FrameBufferManager* framBuffer)
{
	RenderContextDX12 rc;
	rc.d3d_command_list = framBuffer->GetCommandList();
	rc.luminanceExtractionData = bloomData.luminanceExtractionData;
	rc.gaussianFilterData      = bloomData.gaussianFilterData;

	// 高輝度抽出用バッファに描画先を変更して高輝度抽出
	{
		framBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));
		framBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));
		framBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));

		// 高輝度抽出処理
		SpriteShaderDX12* shader = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::LuminanceExtraction);
		renderSprite[static_cast<int>(RenderTarget::Luminance)]->Begin(rc);
		renderSprite[static_cast<int>(RenderTarget::Luminance)]->Draw(
			0, 0,
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance)->GetWidth(),
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance)->GetHeight(),
			0, 0,
			T_GRAPHICS.GetScreenWidth(), 
			T_GRAPHICS.GetScreenHeight(),
			0,
			1, 1, 1, 1,
			framBuffer->GetViewport());
		
		shader->Render(rc, renderSprite[static_cast<int>(RenderTarget::Luminance)].get());

		framBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance));
	}

	// 抽出した高輝度描画対象を暈して書き込む
	{
		framBuffer->WaitUntilToPossibleSetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur));
		framBuffer->SetRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur));
		framBuffer->Clear(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur));

		SpriteShaderDX12* shader = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::GaussianBlur);
		renderSprite[static_cast<int>(RenderTarget::GaussianBlur)]->Begin(rc);
		renderSprite[static_cast<int>(RenderTarget::GaussianBlur)]->Draw(
			0, 0,
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur)->GetWidth(),
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur)->GetHeight(),
			0, 0,
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance)->GetWidth(),
			T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::Luminance)->GetHeight(),
			0,
			1, 1, 1, 1,
			framBuffer->GetViewport());
		shader->Render(rc, renderSprite[static_cast<int>(RenderTarget::GaussianBlur)].get());

		framBuffer->WaitUntilFinishDrawingToRenderTarget(T_GRAPHICS.GetFramBufferDX12(FrameBufferDX12Id::GaussianBlur));
	}

	// 元のバッファに戻す
	{
		D3D12_VIEWPORT view = T_GRAPHICS.GetViwePort();
		framBuffer->SetViewportAndScissor(view);
		framBuffer->SetRenderTarget(T_GRAPHICS.GetCurrentFrameBuffuerRTV(), T_GRAPHICS.GetCurrentFrameBuffuerDSV());
	}

	// 抽出した高輝度描画対象を書き込む
	{
		SpriteShaderDX12* shader = T_GRAPHICS.GetSpriteShaderDX12(SpriteShaderDX12Id::Finalpass);
		renderSprite[static_cast<int>(RenderTarget::Finalpass)]->Begin(rc);
		renderSprite[static_cast<int>(RenderTarget::Finalpass)]->Draw(
			0, 0,
			T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
			0, 0,
			T_GRAPHICS.GetScreenWidth(), T_GRAPHICS.GetScreenHeight(),
			0,
			1, 1, 1, 1,
			framBuffer->GetViewport());
		
		rc.finalpassnData.bloomTexture = renderSprite[static_cast<int>(RenderTarget::GaussianBlur)]->GetDescriptor();
		rc.colorGradingData = colorGradingData;

		shader->Render(rc, renderSprite[static_cast<int>(RenderTarget::Finalpass)].get());
	}
}

//******************************************************************
// @brief       デバッグ情報の表示
// @param[in]   なし
// @return      なし
//******************************************************************
void PostprocessingRendererDX12::DrawDebugGUI()
{
	if (ImGui::TreeNode("PostProcess"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::SliderInt("kernelSize", &bloomData.gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
			ImGui::SliderFloat("deviation", &bloomData.gaussianFilterData.deviation, 1.0f, 10.0f);
			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
			ImGui::TreePop();
		}
		SetBloom(bloomData);

		if (ImGui::TreeNode("ColorGrading"))
		{
			ImGui::SliderFloat("hueShift", &colorGradingData.hueShift, 0.0f, 360.0f);
			ImGui::SliderFloat("saturation", &colorGradingData.saturation, 0.0f, 2.0f);
			ImGui::SliderFloat("brightness", &colorGradingData.brightness, 0.0f, 2.0f);
			ImGui::TreePop();
		}
		SetColoGrading(colorGradingData);

		ImGui::TreePop();
	}
}