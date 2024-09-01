// 平行光源情報
struct DirectionalLightData
{
    float4 direction; // 向き
    float4 color; // 色
};

// 点光源情報
struct PointLightData
{
    float4 position; // 座標
    float4 color; // 色
    float range; // 範囲
    float3 dummy;
};

// 点光源の最大数
static const int PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
    float4 position; // 座標
    float4 direction; // 向き
    float4 color; // 色
    float range; // 範囲
    float innerCorn;
    float outerCorn;
    float dummy;
};
// スポットライトの最大数
static const int SpotLightMax = 8;

//--------------------------------------------
//	ランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
// 返す値		: 拡散反射色
float3 CalcLambertDiffuse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
	// 法線とライト方向へのベクトルでの内積
	// 内積した結果、ライトから見て裏向きの面だとマイナスの結果が出てしまうため
	// それをそのまま乗算すると色がおかしくなります。
	// 0以下であれば0になるように制限をつけましょう。
    float d = max(dot(normal, -lightVector), 0);

	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
    return lightColor * d * kd;
}

//--------------------------------------------
//	フォンの鏡面反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// eyeVector		: 視線ベクトル(正規化済み)
// shininess		: 光沢度(質感調整値)
// ks			: 反射率(反射の強さ)
// 返す値		: 鏡面反射色
float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
{
	// 入射ベクトルを法線方向に対して反射させた反射ベクトルを算出しましょう
    float3 R = reflect(lightVector, normal);

	// 反射ベクトルと視線ベクトルとで内積
	// 内積した結果を0以下であれば0になるように制限をつけましょう。
    float d = max(dot(R, eyeVector), 0);

	// 光沢処理
	// 反射は表面がツルツルなほどハイライトは強くなり、範囲が小さくなります。
	// 逆に表面がザラザラなほど、ハイライトは弱くなり、範囲は広がります。
	// この変化は指数的に変化するため光沢度を乗数として使用します。
    d = pow(d, shininess);

	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
    return lightColor * d * ks;
}

//--------------------------------------------
//	ハーフランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
float3 ClacHalfLambert(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
    float d = clamp(dot(normal, -lightVector) * 0.5f + 0.5f, 0.0f, 1.0f);
    return lightColor * d * kd;
}

//--------------------------------------------
// リムライト
//--------------------------------------------
// normal		: 法線(正規化済み)
// eyeVector	: 視線ベクトル(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// rimPower		: リムライトの強さ(初期値はテキトーなので自分で設定するが吉)
float3 CalcRimLight(float3 normal, float3 eyeVector, float3 lightVector, float3 lightColor, float rimPower = 3.0f)
{
    float rim = 1.0f - saturate(dot(eyeVector, normal));
    rim = pow(rim, rimPower);

    return lightColor * rim * saturate(dot(eyeVector, lightVector));
}

//--------------------------------------------
//	トゥーン拡散反射計算関数
//--------------------------------------------
// toonTexture		: トゥーン用U方向ランプテクスチャ
// toonSamplerState	: トゥーン用サンプラーステート
// normal			: 法線(正規化済み)
// lightVector		: 入射ベクトル(正規化済み)
// lightColor		: 入射光色
// kd				: 反射率(反射の強さ)
// 返す値			: 拡散反射色
float3 CalcToonDiffuse(Texture2D toonTexture, SamplerState toonSamplerState, float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
	//	法線と入射ベクトルからU座標を求める
    float u = clamp(dot(normal, -lightVector) * 0.4 + 0.6, 0.1, 0.9);

	// トゥーンテクスチャから色を取得する
    float3 c = toonTexture.Sample(toonSamplerState, float2(u, 0.1));

    return lightColor * c * kd;
}