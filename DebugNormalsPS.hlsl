#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    return float4(input.normal, 1);
}