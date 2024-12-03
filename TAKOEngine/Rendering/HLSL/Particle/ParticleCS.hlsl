#define ROOT_SIG "DescriptorTable(SRV(t0)), \
                  DescriptorTable(UAV(u0)), \
                  DescriptorTable(CBV(b0)), \
                  StaticSampler(s0 ,\
                  filter = FILTER_MIN_MAG_MIP_LINEAR,\
                  addressU = TEXTURE_ADDRESS_WRAP,\
                  addressV = TEXTURE_ADDRESS_WRAP,\
                  addressW = TEXTURE_ADDRESS_WRAP,\
                  mipLodBias = 0.0f,\
                  maxAnisotropy = 1,\
                  comparisonFunc  = COMPARISON_NEVER,\
                  borderColor = STATIC_BORDER_COLOR_TRANSPARENT_BLACK,\
                  minLOD = 0.0f,\
                  maxLOD = 3.402823466e+38f,\
                  space = 0)"

struct Vertex
{
    float3 position;
    float3 param;
    float4 color;
    float3 velocity;
    float Life;
    float LifeTimae; //寿命
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    
    float ElapsedTime; // 経過時間
    float DeltaTime;   // フレーム経過時間
    int EmitCount;     // 出現させる個数
    int EmitIndex;     // 現在の出現待ちインデックス
    float4 StartColor;
    float4 EndColor;
    
    float Scale;
    float Lifetime;
    float NoiseSpeed;
    float NoisePower;
};

Texture2D    NoiseTex : register(t0);
SamplerState sampler_repeat : register(s0);

//UAV slot0(頂点バッファ)
RWStructuredBuffer<Vertex> buf : register(u0);

// ランダム
float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

[RootSignature(ROOT_SIG)]
[numthreads(256, 1, 1)]  // スレッド数
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x; //スレッドのx = 頂点番号
    
    Vertex src = buf[index];
    Vertex dst = (Vertex) 0;
    
    //頂点座標取得
    //バッファのIndex番目からfloatを3つ分とってくる
    float3 position = src.position;
    float3 param    = src.param;
    float4 color    = src.color;
    float3 velocity = src.velocity;
    float life      = src.Life;
    float lifeTime  = src.LifeTimae;
    
    //ノイズ処理
    float2 uv;
    uv.x = index / 256.0;
    uv.y = life * NoiseSpeed; //ノイズのスピード
    
    float4 noise = NoiseTex.SampleLevel(sampler_repeat, uv, 0);
    
    velocity += (noise - 0.5) * DeltaTime * NoisePower; //ノイズの力
    
    //生存時間
    life += DeltaTime;
    if (life >= lifeTime)
    {
        param.x = 0; //スケール0
    }
    
    //リセット判定
    if (DTid.x >= EmitIndex && DTid.x < EmitIndex + EmitCount)
    {
        position   = float3(0, 0, 0);
        velocity.x = (rand(float2(life, ElapsedTime)) - 0.5) * 3.0;
        velocity.y = 12.0f;
        velocity.z = (rand(float2(life, ElapsedTime) * 0.2) - 0.5) * 3.0;
        param.x    = Scale; //スケール
        lifeTime   = Lifetime; //寿命
        life       = 0;
    }
    
    //寿命の割合
    float liferate = life / lifeTime;
    param.y = liferate;
    
    //カラー決定
    color = lerp(StartColor, EndColor, liferate);
    
    //処理
    position += velocity * DeltaTime;
    velocity.y -= 0.018 * DeltaTime;
    
    //頂点格納
    //positionをByteの配列として格納
    dst.position  = position;
    dst.param     = param;
    dst.color     = color;
    dst.velocity  = velocity;
    dst.Life      = life;
    dst.LifeTimae = lifeTime;
    
    buf[index] = dst;
}