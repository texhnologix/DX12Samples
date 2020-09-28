#include "Common.hlsli"


ConstantBuffer<Constants> Globals       : register(b0);
ConstantBuffer<MeshInfo>  MeshInfo      : register(b1); 
StructuredBuffer<Meshlet> Meshlets      : register(t0);
StructuredBuffer<Vertex>  Vertices      : register(t1);
StructuredBuffer<uint>    VertexIndices : register(t2);
StructuredBuffer<uint>    PrimitiveIndices   : register(t3);

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
  in uint threadId : SV_GroupThreadID,
  in uint groupId  : SV_GroupID,
  in  payload PayloadData payload,
  out vertices VertexOut verts[128],
  out indices uint3 tris[256],
  out primitives PrimitiveOut prims[256]
)
{
  uint meshletIndex = payload.MeshletIndices[groupId];

  Meshlet m = Meshlets[meshletIndex];

  uint faceCount = m.PrimsCountAndOffset & 0xFF;
  uint faceOffset = (m.PrimsCountAndOffset >> 8) & 0xFFFFFF;

  uint vertexCount = m.VertsCountAndOffset & 0xFF;
  uint vertexOffset = (m.VertsCountAndOffset >> 8) & 0xFFFFFF;

  SetMeshOutputCounts(vertexCount, faceCount * 2);
  if (threadId < vertexCount) {
    uint vindex = vertexOffset + threadId;
    uint wordOffset = (vindex & 0x1);
    uint byteOffset = (vindex >> 1);

    uint indexPair = VertexIndices[byteOffset];
    uint index = (indexPair >> (wordOffset << 4)) & 0xFFFF;

    Vertex v = Vertices[index];

    VertexOut vout;
    vout.Position = mul(float4(v.Position, 1), Globals.WorldViewProj);
    vout.UV = v.UV;
    vout.MeshletIndex = uint2(threadId, groupId);
    verts[threadId] = vout;
  }

  if (threadId < faceCount) {
    uint index = PrimitiveIndices[faceOffset + threadId];
    uint i0 = (index >> 0) & 0xFF;
    uint i1 = (index >> 8) & 0xFF;
    uint i2 = (index >> 16) & 0xFF;
    uint i3 = (index >> 24) & 0xFF;

    tris[threadId * 2 + 0] = uint3(i0, i1, i2);
    tris[threadId * 2 + 1] = uint3(i0, i2, i3);
  }
}
