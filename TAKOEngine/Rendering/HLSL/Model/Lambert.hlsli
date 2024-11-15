#include	"../Lighting/Light.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 w_position : POSITION;
    float4 w_normal : NORMAL;
    float4 w_tangent : TANGENT;
    float2 texcoord : TEXCOORD;
};

cbuffer PRIMITIVE_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    int material;
    bool has_tangent;
    int skin;
    int pad;
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    float4 cameraPosition;

    //ライト情報
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax];   // 点光源情報
    SpotLightData spotLightData[SpotLightMax];      // スポットライト情報
    int pointLightCount;                            // 点光源数
    int spotLightCount;                             // スポットライト数
}

static const uint PRIMITIVE_MAX_JOINTS = 512;
cbuffer PRIMITIVE_JOINT_CONSTANTS : register(b2)
{
    row_major float4x4 joint_matrices[PRIMITIVE_MAX_JOINTS];
};