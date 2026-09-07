// Basic3D_VS.hlsl

cbuffer FrameConstants : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer ObjectConstants : register(b1)
{
    matrix world;
};
cbuffer BaseConstants : register(b2)
{
    float4 Timer;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
    float4 WorldPos : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.WorldPos = mul(input.Pos, world);
    output.Pos = mul(output.WorldPos, view);
    output.Pos = mul(output.Pos, projection);
    output.Normal = mul(input.Normal, (float3x3)world); // Transform normal to world space
    output.Tex = input.Tex;
    return output;
}
