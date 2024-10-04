//--------------------------
// NOTE:①ルートシグネチャの設定⑤
//--------------------------
#define ROOT_SIG "DescriptorTable(SRV(t0)), \
                  DescriptorTable(UAV(u0)), \
                  DescriptorTable(CBV(b0))"

struct Vertex
{
	float3 position;
	float3 normal;
	float3 tangent;
	float2 texcoord;
	float4 color;
	float4 bone_weights;
	uint4  bone_indices;
};

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4	world_transform;
    row_major float4x4	bone_transforms[255];
};

StructuredBuffer<Vertex>	src_vertices : register(t0);
RWStructuredBuffer<Vertex>	dst_vertices : register(u0);

#define CS_THREAD_NUM 8

//--------------------------
// NOTE:①ルートシグネチャの設定⑥
//--------------------------
[RootSignature(ROOT_SIG)]
[numthreads(CS_THREAD_NUM, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint vertex_index = dtid.x;
    Vertex src = src_vertices[vertex_index];
    Vertex dst = (Vertex)0;

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        p += (src.bone_weights[i] * mul(float4(src.position, 1), bone_transforms[src.bone_indices[i]])).xyz;
        n += (src.bone_weights[i] * mul(float4(src.normal, 0), bone_transforms[src.bone_indices[i]])).xyz;
        t += (src.bone_weights[i] * mul(float4(src.tangent, 0), bone_transforms[src.bone_indices[i]])).xyz;
    }
    dst.position = p;
    dst.normal = n;
    dst.tangent = t;
    dst.texcoord = src.texcoord;
    dst.color = src.color;

    dst_vertices[vertex_index] = dst;
}