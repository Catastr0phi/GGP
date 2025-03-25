#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 textureScale;
    float2 textureOffset;
}

Texture2D SurfaceTexture : register(t0);
Texture2D OverlayTexture : register(t1);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv * textureScale + textureOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv) * OverlayTexture.Sample(BasicSampler, uv);
	
    return colorTint * surfaceColor;
}