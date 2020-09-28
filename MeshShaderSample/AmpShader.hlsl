#include "Common.hlsli"

groupshared PayloadData	s_Payload;

ConstantBuffer<Constants> Globals			: register(b0);
ConstantBuffer<MeshInfo>  MeshInfo		: register(b1);
StructuredBuffer<Meshlet> Meshlets		: register(t0);

[NumThreads(32, 1, 1)]
void main(
	in uint threadId  : SV_GroupThreadID,
	in uint dispathId : SV_DispatchThreadID, 
	in uint groupId   : SV_GroupID)
{

	bool visible = false;

	if (dispathId < MeshInfo.MeshletCount)
	{
		visible = true;
	}

	if (visible)
	{
		uint index = WavePrefixCountBits(visible);
		s_Payload.MeshletIndices[index] = dispathId;
	}

	int visibleCount = WaveActiveCountBits(visible);

	DispatchMesh(visibleCount, 1, 1, s_Payload);
}
