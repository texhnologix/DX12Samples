#include "Common.hlsli"

groupshared PayloadData	payload;

StructuredBuffer<Meshlet> Meshlets          : register(t0);

[NumThreads(1, 1, 1)]
void main(in uint gid : SV_GroupID)
{

	payload.Offset[0] = float3(-0.5f, +0.5f, 0.0f);
	payload.Offset[1] = float3(+0.5f, +0.5f, 0.0f);
	payload.Offset[2] = float3(-0.5f, -0.5f, 0.0f);
	payload.Offset[3] = float3(+0.5f, -0.5f, 0.0f);

	DispatchMesh(4, 1, 1, payload);
}
