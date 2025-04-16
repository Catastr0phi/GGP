#include "ShaderStructs.hlsli"
#include "PBRFuncs.hlsli"

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

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
SamplerState BasicSampler : register(s0);

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 directionalLight(Light light, float3 normal, float3 V, float3 surfaceColor, float3 direction, float roughness, float3 specColor, float metalness)
{
    // Diffuse calculation
    float3 diffuse = DiffusePBR(normal, direction);
    
    // Specular calcualtion
    float3 R = reflect(normalize(direction), normal);
    float3 F;
    float3 specular = MicrofacetBRDF(normal, direction, V, roughness, specColor, F);
    
    // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diffuse, F, metalness);
    // Combine the final diffuse and specular values for this light
    float3 total = (balancedDiff * surfaceColor + specular) * light.Intensity * light.Color;
    
    return total;
}

float3 pointLight(Light light, float3 normal, float3 V, float3 surfaceColor, float3 worldPos, float roughness, float3 specColor, float metalness)
{
    // Get direction
    float3 direction = normalize(worldPos - light.Position);
    
    // Call directional light with found direction and attenuate
    return directionalLight(light, normal, V, surfaceColor, direction, roughness, specColor, metalness) * Attenuate(light, worldPos);
}

float3 spotLight(Light light, float3 normal, float3 V, float3 surfaceColor, float3 worldPos, float roughness, float3 specColor, float metalness)
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
    return pointLight(light, normal, V, surfaceColor, worldPos, roughness, specColor, metalness) * spotTerm;
}

float4 main(VertexToPixel input) : SV_TARGET
{   
    float2 uv = input.uv * textureScale + textureOffset;
    float4 surfaceColor = pow(Albedo.Sample(BasicSampler, uv), 2.2f) * colorTint;
    
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
    
    // Roughness and metallic
    float roughness = RoughnessMap.Sample(BasicSampler, uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, uv).r;
    
    // Specular color determination 
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);
    
    // Light calculation
    float3 V = normalize(camPosition - input.worldPos);
    
    for (int i = 0; i < lightCount; i++)
    {
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += directionalLight(lights[i], input.normal, V, surfaceColor.xyz, lights[i].Direction, roughness, specularColor, metalness);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += pointLight(lights[i], input.normal, V, surfaceColor.xyz, input.worldPos, roughness, specularColor, metalness);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += spotLight(lights[i], input.normal, V, surfaceColor.xyz, input.worldPos, roughness, specularColor, metalness);
                break;
        }
    }

    	
    return float4(pow(totalLight, 1.0f/2.2f), 1);
}