#include "Grass.hlsli"

HS_CONSTANT_DATA_OUTPUT patch_constant_function(InputPatch<VS_CONTROL_POINT_OUTPUT,CONTROL_POINT_COUNT> input_patch, uint patch_id : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    const float subdivision = tesselation_max_subdivision;
    output.tess_factor[0] = subdivision;
    output.tess_factor[1] = subdivision;
    output.tess_factor[2] = subdivision;
    output.inside_tess_factor = subdivision;
    
    return output;
}

[domain("tri")]
[partitioning("integer")] 
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("patch_constant_function")]
HS_CONTROL_POINT_OUTPUT main(
    InputPatch<VS_CONTROL_POINT_OUTPUT, CONTROL_POINT_COUNT> input_patch,
    uint output_control_point_id : SV_OutputControlPointID,
    uint primitive_id : SV_PrimitiveID)
{
    HS_CONTROL_POINT_OUTPUT output;
    output = input_patch[output_control_point_id];
    
    return output;
}
