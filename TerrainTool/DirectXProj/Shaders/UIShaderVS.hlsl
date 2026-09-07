cbuffer FrameConstants : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer ObjectConstants : register(b1)
{
    matrix world;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // For UI, the 'world' matrix will contain the screen-space position and scale.
    // The 'projection' matrix will be an orthographic projection matching the screen resolution.
    // The 'view' matrix is not needed and will be an identity matrix.
    output.pos = mul(float4(input.pos, 1.0f), world);
    output.pos = mul(output.pos, projection);

    output.uv = input.uv;

    return output;
}