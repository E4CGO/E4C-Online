//---------------------------------------------
//      シャドウマップ参照用情報の計算
//---------------------------------------------
// worldPosition       : ワールド座標
// lightViewProjection : ライトビュープロジェクション行列
// 返す値               : シャドウマップ参照用のUV座標及び深度情報
float3 CalcShadowTexcoord(float3 worldPosition, matrix lightViewProjection)
{
	//クリップ空間の座標に変換
	float4 worldViewProjectionPosition = mul(float4(worldPosition, 1), lightViewProjection);

	//透視除算してNDC座標に変換
	worldViewProjectionPosition /= worldViewProjectionPosition.w;

	//NDC座標のXY座標をUV座標に変換
	//Z値はすでに0 ~ 1の範囲に変換されているので、何もしなくて良い
	worldViewProjectionPosition.y = -worldViewProjectionPosition.y;
	worldViewProjectionPosition.xy = 0.5f * worldViewProjectionPosition.xy + 0.5f;
	
	return worldViewProjectionPosition.xyz;
}

//----------------------------------------------------
//     シャドウマップから深度値を取得して影かどうかを渡す
//----------------------------------------------------
// tex            : シャドウマップ
// samplerState   : サンプラーステート
// shadowTexcoord : シャドウマップ参照用情報
// shadowColor    : 影の色
// shadowBias     : 深度比較用のオフセット値
// 返す値          :影かどうか
float3 CalcShadowColor(Texture2D tex, SamplerState samplerState, float3 shadowTexcoord, float3 shadowColor, float shadowBias)
{
	//シャドウマップから深度値取得
	float depth = tex.Sample(samplerState, shadowTexcoord.xy).r;

	//深度値を比較して影かどうかを判定する
	float shadow = step(shadowTexcoord.z - depth, shadowBias);

	return lerp(shadowColor, 1, shadow);
}

//-------------------------------------------------
//      PCFフィルター付きソフトシャドウマップ
//-------------------------------------------------
//tex             : シャドウマップ
//samplerState	  : サンプラーステート
//shadowTexcoord  : シャドウマップ参照用情報
//shadowBias	  : 深度比較用のオフセット値
//shadowColor	  : 影の色
//返す値			  : 影の色
float3 CalcShadowColorPCFFilter(Texture2D tex, SamplerState samplerState, float3 shadowTexcoord, float3 shadowColor, float shadowBias)
{
	//テクセルサイズの計算
	float2 texelSize;
	{
		//テクスチャの縦幅横幅を取得する
		uint width, height;
		tex.GetDimensions(width, height);

		//算出
		texelSize = float2(1.0f / width, 1.0f / height);
	}

	float factor = 0;
	static const int PCFKernelSize = 5;    //指定は奇数にすること

	for (int x = -PCFKernelSize / 2; x <= PCFKernelSize / 2; ++x)
	{
		for (int y = -PCFKernelSize / 2; y <= PCFKernelSize / 2; ++y)
		{
			//シャドウマップから深度値取得
			float depth = tex.Sample(samplerState, shadowTexcoord.xy + texelSize * float2(x, y)).r;
			factor += step(shadowTexcoord.z - depth, shadowBias);
		}
	}

	//深度値を比較して影かどうかを判定する
	return lerp(shadowColor, 1, factor / (PCFKernelSize * PCFKernelSize));
}