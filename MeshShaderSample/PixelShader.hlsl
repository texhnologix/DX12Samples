#include "Common.hlsli"

Texture2D<float4>	Tex0 : register(t0);
SamplerState			Smp0 : register(s0);

float4 main(VertexOut Input) : SV_TARGET
{
	return (Tex0.Sample(Smp0, Input.UV));
}
