#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix proj;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;
    
    // Zero out view translation
    matrix viewNoTranslation = view;
    
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    // Apply view and projection
    output.screenPosition = mul(mul(proj, viewNoTranslation), float4(input.localPosition, 1.0f));
    output.screenPosition.z = output.screenPosition.w;
    
    // Sampl Direction is just the position
    output.sampleDir = input.localPosition;
    
    return output;
}