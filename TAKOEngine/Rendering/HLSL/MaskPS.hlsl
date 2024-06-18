#include "Mask.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0, pin.texcoord) * pin.color;

	// マスク画像から赤色を取得
	float mask = maskTexture.Sample(sampler0, pin.texcoord).r;

	// step関数を用いてmaskの値とdissolveThresholdの値を比較して透過値を0 or 1にする
	float alpha = step(mask, dissolveThreshold);

	// 縁の処理
	float edgeValue = step(dissolveThreshold, mask)
					* step(mask, dissolveThreshold + edgeThreshold)
					* step(mask - dissolveThreshold, dissolveThreshold);

	color.rgb += (edgeColor.rgb * ((mask - dissolveThreshold) / mask) - color.rgb) * step(1, edgeValue);

	alpha = saturate(alpha + edgeValue);

	// colorの透過値に乗算する
	color.a *= alpha;

	// アルファが0以下ならそもそも描画しないようにする
	clip(color.a - 0.01f);

	return color;
}