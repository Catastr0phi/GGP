cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float time;
}
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition   : SV_POSITION;
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
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