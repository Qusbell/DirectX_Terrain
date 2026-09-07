// PBR_PS.hlsl


// Input from vertex shader
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION0;
    float3 Normal : NORMAL0;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : BITANGENT0;
};


cbuffer FrameConstants : register(b0)
{
    matrix view;
    matrix projection;
};

// Material properties
cbuffer MaterialConstants : register(b3)
{
    float2 Tiling;
    float2 Offset;
};

// Lighting properties
cbuffer LightConstants : register(b2)
{
    float3 LightDirection;
    float  _padding1;
    float4 LightColor;
    float3 CameraPosition;
    float  _padding2;
};

// Textures and Sampler
Texture2D    AlbedoTexture : register(t0);
Texture2D    NormalTexture : register(t1);
Texture2D    MetallicRoughnessTexture : register(t2); // R: Metallic, G: Roughness
SamplerState AnisotropicSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    // 1. Normalize interpolated vectors
    float3 normal = normalize(input.Normal);
    float3 tangent = normalize(input.Tangent);
    float3 bitangent = normalize(input.Bitangent);

    // 2. Apply UV transformations
    float2 uv = input.Tex * Tiling + Offset;

    // 3. Sample Textures
    float4 albedoColor = AlbedoTexture.Sample(AnisotropicSampler, uv);
    float3 normalMapSample = NormalTexture.Sample(AnisotropicSampler, uv).rgb * 2.0 - 1.0; // Unpack from [0,1] to [-1,1]
    float2 metallicRoughness = MetallicRoughnessTexture.Sample(AnisotropicSampler, uv).rg;
    float metallic = metallicRoughness.r;
    float roughness = metallicRoughness.g;

    // 4. Calculate world normal using normal map
    float3x3 tbn = float3x3(tangent, bitangent, normal);
    float3 worldNormal = normalize(mul(normalMapSample, tbn));

    // 5. Basic Lighting Calculations
    float3 lightDir = -normalize(LightDirection);
    float3 viewDir = normalize(CameraPosition - input.WorldPos);

    // Diffuse (Lambertian)
    float nDotL = saturate(dot(worldNormal, lightDir));
    float3 diffuse = albedoColor.rgb * nDotL * LightColor.rgb;

    // Specular (Blinn-Phong approximation)
    float3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(saturate(dot(worldNormal, halfwayDir)), (1.0 - roughness) * 256.0);
    float3 specular = spec * LightColor.rgb;

    // Combine lighting
    // A simple lerp between diffuse and specular based on metallic property
    float3 finalColor = lerp(diffuse, specular, metallic);

    // Add a simple ambient light so dark areas are not pure black
    float3 ambient = 0.1f * albedoColor.rgb;
    finalColor += ambient;

    
    //finalColor = albedoColor.xyz;
    return float4(finalColor, albedoColor.a);
}
