cbuffer CBuffer_WVP : register(b0)
{
    matrix WorldViewProjection;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), WorldViewProjection);
    output.texcoord = input.texcoord;
    return output;
}
