// Basic3D_PS.hlsl

Texture2D albedoTexture : register(t0);
SamplerState basicSampler : register(s0)
{
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

// dx11 에서 이렇게 지정 하는것 안됨
// https://unialgames.tistory.com/entry/DirectX11Sampler
SamplerState SamplerAddressWrap
{
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

cbuffer BaseConstants : register(b3)
{
    float4 Timer;
};
cbuffer LightConstants : register(b2)
{
    float3 LightDirection;
    float  _padding1;
    float4 LightColor;
    float3 CameraPosition;
    float  _padding2;
};



struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
    float4 WorldPos : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Normalize vectors
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(LightDirection);

    // Calculate diffuse light
    float diffuse = saturate(dot(normal, -lightDir));

    
    float2 uv = input.Tex;
    uv.x += Timer.x;
    // Sample the texture
    float4 albedo = albedoTexture.Sample(basicSampler, uv);
    //tex2D()

    // Combine lighting and texture color
    float3 finalColor = albedo.rgb * LightColor.rgb * diffuse;

    finalColor.xyz = albedo.rgb;
    return float4(finalColor, albedo.a);
}


