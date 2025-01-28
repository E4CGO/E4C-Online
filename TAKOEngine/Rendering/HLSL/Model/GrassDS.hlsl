#include "Grass.hlsli" 

struct HS_CONSTANT_DATA_OUTPUT
{
    float tess_factor[3] : SV_TessFactor;
    float inside_tess_factor : SV_InsideTessFactor;
};

#define CONTROL_POINT_COUNT 3

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input,
    float3 domain : SV_DomainLocation, 
    const OutputPatch<HS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> patch)
{
    DS_OUTPUT output;

    output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
    output.position.w = 1;

    output.normal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;
    output.normal.w = 0;
    
    output.tangent = patch[0].tangent * domain.x + patch[1].tangent * domain.y + patch[2].tangent * domain.z;
    output.tangent.w = 0;
    
    return output;
}
