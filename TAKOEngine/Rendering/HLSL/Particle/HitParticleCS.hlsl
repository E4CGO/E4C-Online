#define ROOT_SIG "DescriptorTable(UAV(u0)), \
                  DescriptorTable(CBV(b0))"

#define NUMTHREADS_X 16

struct particle
{
    int state; // 1:alive, 0:dead
	
    float4 color;
    float3 position; // ワールド空間座標
    float3 velocity; // ワールド空間速度
    float size;
	
    float age;
    float lifespan;
	
    uint chip;
};

cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    
    float4 emission_position;   // 放射位置
    float  time;                // 時間
    float  delta_time;          // 削除時間
    uint   max_particle_count;  // パーティクルの最大数
    int    dummy;
    
    uint2 sprite_sheet_grid;    // 画像タイルセット内の格子状のマス目
    float respawn_radius;       // リスポーン半径
};

RWStructuredBuffer<particle> particle_buffer : register(u0);

#define PI 3.14159265358979
float rand(float n)
{
	// 戻り値は0以上1未満
    return frac(sin(n) * 43758.5453123);
}

void spawn(in uint id, inout particle p)
{
    float f0 = rand((float) id / max_particle_count * 2.0f * PI);
    float f1 = rand(f0 * 2.0f * PI);
    float f2 = rand(f1 * 2.0f * PI);
    float f3 = rand(f2 * 2.0f * PI);

	// 緯度θを範囲[0, π]で生成
    float theta = acos(2.0f * f0 - 1.0f);
    
    // 経度φを範囲[0, 2π]で生成
    float phi = 2.0f * PI * f1;
	
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);
    float sin_phi = sin(phi);
    float cos_phi = cos(phi);
	
    float radius = 2.0f * f0 + (respawn_radius * 3);

    p.position.x = emission_position.x + radius * sin_theta * cos_phi;
    p.position.y = emission_position.y + radius * sin_theta * sin_phi;
    p.position.z = emission_position.z + radius * cos_theta;
    
    p.velocity.x = 0.0f;
    p.velocity.y = 0.0f;
    p.velocity.z = 0.0f;
	
    p.size = 0.005f;
    p.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    p.age = -1.0f * f0;
    p.lifespan = 0.2f;
	
    int count = sprite_sheet_grid.x * sprite_sheet_grid.y;
    p.chip = f3 * count;
}

[RootSignature(ROOT_SIG)]
[numthreads(NUMTHREADS_X, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = DTid.x;
    
    particle p = particle_buffer[id];
    
    float3 d = emission_position.xyz - p.position;
    float  r = max(0.1f, length(d));
    
    float  G = 60.0f;            // 重力定数
    float3 g = G / (r * r) * d;
    
    p.velocity += g * delta_time;
    p.position += p.velocity * delta_time;    // 位置更新
    p.color.rgb = pow(p.color.rgb, length(d));
    
    p.color.r = p.age / p.lifespan * 0.8f;
    p.color.g = p.age / p.lifespan * 0.3f;
    p.color.b = p.age / p.lifespan * 0.1f;
    
    p.age += delta_time;
    if (length(d) < respawn_radius) // 寿命が切れたら
    {
        spawn(id, p);  // リスポーン
    }
    
    particle_buffer[id] = p;
}