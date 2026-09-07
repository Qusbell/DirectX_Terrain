Texture2D    SpriteTexture : register(t0);
SamplerState Sampler       : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return SpriteTexture.Sample(Sampler, input.texcoord);
}
