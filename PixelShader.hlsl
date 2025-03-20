cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float2 textureScale;
    float2 textureOffset;
    float3 camPosition;
}

Texture2D SurfaceTexture : register(t0); 
SamplerState BasicSampler : register(s0); 

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
	float4 screenPosition	: SV_POSITION;
    float2 uv				: TEXCOORD;
    float3 normal			: NORMAL;
    float3 worldPos : POSITION;
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
    float2 uv = input.uv * textureScale + textureOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv);
    
    // In-class lighting demo
    /*float3 totalLight = float3(0, 0, 0);
    
    // Ambient
    float3 ambientColor = float3(0.1f, 0.0f, 0.2f);
    float3 ambientTerm = surfaceColor * ambientColor;
    
    // Light dir
    float3 lightColor = float3(1, 1, 1);
    float lightIntensity = 1.0f;
    float3 lightDirection = float3(1, 0, 0);
    
    // Diffuse calculation
    float3 diffuseTerm = max(dot(input.normal, -lightDirection), 0) * lightIntensity * lightColor * surfaceColor;
    
    // Specular calculation
    float3 refl = reflect(lightDirection, input.normal);
    float3 viewVector = normalize(camPosition - input.worldPos);
    
    float3 specTerm = pow(max(dot(refl, viewVector), 0), 256) *
        lightColor * lightIntensity * surfaceColor;
    
    // Add lighting
    totalLight += ambientTerm + diffuseTerm + specTerm;*/
	
    return colorTint * surfaceColor;
}