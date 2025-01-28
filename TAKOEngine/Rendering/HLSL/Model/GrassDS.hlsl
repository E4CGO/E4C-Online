#include "Grass.hlsli" 

[domain("tri")]
DS_OUT main(HS_CONSTANT_DATA_OUTPUT input, 
    float3 domain : SV_DomainLocation, 
    const OutputPatch<HS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> patch)
{
    DS_OUT output;

    output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
    output.position.w = 1;

    return output;
}
