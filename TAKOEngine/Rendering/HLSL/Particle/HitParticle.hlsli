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

struct VS_OUT
{
    uint vertex_id : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(SRV(t0), visibility=SHADER_VISIBILITY_ALL), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_ALL)"