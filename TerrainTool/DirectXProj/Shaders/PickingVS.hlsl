
cbuffer FrameConstants : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer ObjectConstants : register(b1)
{
    matrix world;
    float4 id;
};

struct VS_INPUT
{
    float4 pos : POSITION;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 id : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.id = id;
    return output;
}