Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return txDiffuse.Sample(samLinear, input.uv);
}