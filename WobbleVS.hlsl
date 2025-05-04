cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float time;
}

struct VertexShaderInput
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel output;
    
    // Wobble the x value based on the y value
    float3 wobblePos = float3(input.localPosition.x + sin(input.localPosition.y * 3 + time * 5)/2, input.localPosition.yz);

	// Create world-view-projection matrix from camera matrices
    matrix wvp = mul(proj, mul(view, world));
    output.screenPosition = mul(wvp, float4(wobblePos, 1.0f));

	// Send uv and normal to next stage unchanged
    output.worldPos = mul(world, float4((input.localPosition), 1.0f)).xyz;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}