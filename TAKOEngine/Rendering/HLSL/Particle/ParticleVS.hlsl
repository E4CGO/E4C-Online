#include "Particle.hlsli"

[RootSignature(ROOT_SIG)]
GSInput main(VSInput input)
{
    GSInput output;
    output.Position = input.Position;
    output.Param = input.Param;
    output.Color = input.Color;
    
    return output;
}