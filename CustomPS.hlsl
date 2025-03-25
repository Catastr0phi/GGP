#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float time;
}

float4 main(VertexToPixel input) : SV_TARGET
{        
    // Pixelation factor: Lower value means less possible colors, so a more pixelated look
    float pixelation = 5;
    
    // Takes sin and cos of the uv, which change over time. 
    float uvSin = sin((input.uv.x + time / 5) * 50) * 0.5 + 0.5;
    float uvCos = cos((input.uv.y + time / 5) * 50) * 0.5 + 0.5;
    
    // Quantizing
    // Multiplied by a pixelation factor, floored, then divided by that pixelation factor
    float quantX = floor(uvSin * pixelation)/pixelation;
    float quantY = floor(uvCos * pixelation)/pixelation;
    
    // Divided by the pixelation factor
    // G is average of R and B
    return float4(quantX, (quantX + quantY) / 2, quantY, 1);
}