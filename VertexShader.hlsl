#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix worldInvTrans;
    matrix view;
    matrix proj;
    matrix lightView;
    matrix lightProj;
}

VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// Create world-view-projection matrix from camera matrices
    matrix wvp = mul(proj, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	
    output.worldPos = mul(world, float4((input.localPosition), 1.0f)).xyz;
    output.uv = input.uv;
    output.normal = mul((float3x3)worldInvTrans, input.normal);
    output.tangent = mul((float3x3)world, input.tangent);
    
    matrix shadowWVP = mul(lightProj, mul(lightView, world));
    output.shadowMapPos = mul(shadowWVP, float4(input.localPosition, 1.0f));

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}