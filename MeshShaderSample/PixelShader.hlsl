#include "Common.hlsli"

Texture2D<float4>	Tex0 : register(t0);
SamplerState			Smp0 : register(s0);

float4 main(VertexOut Input) : SV_TARGET
{
  float scale = (float(Input.MeshletIndex.x & 31) / 31.0f) * 0.75f + 0.25f;;
  uint meshletIndex = Input.MeshletIndex.y;

  float3 diffuseColor = float3(float(meshletIndex & 1), float(meshletIndex & 3) / 4, float(meshletIndex & 7) / 8) * 0.75 + 0.25;

  return float4(diffuseColor * scale, 1);// (Tex0.Sample(Smp0, Input.UV));
}
