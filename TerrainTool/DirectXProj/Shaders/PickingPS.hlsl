
// dx11 에서 이렇게 지정 하는것 안됨
// https://unialgames.tistory.com/entry/DirectX11Sampler
SamplerState SamplerAddressWrap
{
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 id : COLOR;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return input.id;
}