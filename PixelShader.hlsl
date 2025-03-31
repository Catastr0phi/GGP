#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0)
{
    Light lights[MAX_LIGHTS];
    float4 colorTint;
    float2 textureScale;
    float2 textureOffset;
    float3 camPosition;
    float roughness;
    float3 ambient;
    int lightCount;
}

Texture2D SurfaceTexture : register(t0); 
texture2D NormalMap : register(t1);
SamplerState BasicSampler : register(s0); 

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 directionalLight(Light light, float3 normal, float3 V, float exponent, float3 surfaceColor, float3 direction)
{
    // Diffuse calculation
    float3 diffuse = max(dot(normal, normalize(-direction)), 0) * light.Intensity * light.Color;
    
    // Specular calcualtion
    float3 R = reflect(normalize(direction), normal);
    float3 specular = float3(0, 0, 0);
    
    if (exponent > 0.05)
    {
        specular = pow(max(dot(R, V), 0.0f), exponent);
    }
    
    return surfaceColor * (diffuse + specular);
}

float3 pointLight(Light light, float3 normal, float3 V, float exponent, float3 surfaceColor, float3 worldPos)
{
    // Get direction
    float3 direction = normalize(worldPos - light.Position);
    
    // Call directional light with found direction and attenuate
    return directionalLight(light, normal, V, exponent, surfaceColor, direction) * Attenuate(light, worldPos);
}

float3 spotLight(Light light, float3 normal, float3 V, float exponent, float3 surfaceColor, float3 worldPos)
{
    // Get direction
    float3 direction = normalize(worldPos - light.Position);
    
    // Get angle between our direction and light direction
    float angle = saturate(dot(direction, light.Direction));
    
    // Get cosines and calc range
    float cosInner = cos(light.SpotInnerAngle);
    float costOuter = cos(light.SpotOuterAngle);
    float falloffRange = costOuter - cosInner;
    
    // How much this pixel should be lit based on angles
    float spotTerm = saturate((costOuter - angle) / falloffRange);
    
    // Use pointLight function and multiply by term
    return pointLight(light, normal, V, exponent, surfaceColor, worldPos) * spotTerm;
}

float4 main(VertexToPixel input) : SV_TARGET
{   
    float2 uv = input.uv * textureScale + textureOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, uv) * colorTint;
    
    // Unpack normal map
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    
    // Normalize input vectors
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    
    // Calculate TBN
    input.tangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal));
    float3 B = cross(input.tangent, input.normal);
    float3x3 TBN = float3x3(input.tangent, B, input.normal);

    // Transform normal from map
    input.normal = mul(unpackedNormal, TBN);
    
    float3 totalLight = float3(0, 0, 0);
    
    // Ambient
    float3 ambientTerm = surfaceColor.xyz * ambient;
    totalLight += ambientTerm;
    
    // Light calculation
    float3 V = normalize(camPosition - input.worldPos);
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    for (int i = 0; i < lightCount; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += directionalLight(lights[i], input.normal, V, specExponent, surfaceColor.xyz, lights[i].Direction);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += pointLight(lights[i], input.normal, V, specExponent, surfaceColor.xyz, input.worldPos);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += spotLight(lights[i], input.normal, V, specExponent, surfaceColor.xyz, input.worldPos);
                break;
        }
    }

    	
    return float4(totalLight, 1);
}