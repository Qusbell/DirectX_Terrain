// PBR_VS.hlsl


// Per-frame constants

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
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION0;
    float3 Normal : NORMAL0;
    float2 Tex : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : BITANGENT0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // Transform position to world space and then to clip space
    output.WorldPos = mul(float4(input.Pos, 1.0f), world).xyz;
    output.Pos = mul(float4(output.WorldPos, 1.0f), view);
    output.Pos = mul(output.Pos, projection);

    // Transform vectors to world space
    output.Normal = mul(input.Normal, (float3x3)world);
    output.Tangent = mul(input.Tangent, (float3x3)world);
    output.Bitangent = mul(input.Bitangent, (float3x3)world);

    // Pass through texture coordinates
    output.Tex = input.Tex;
    

    return output;
}
