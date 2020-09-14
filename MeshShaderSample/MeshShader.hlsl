#include "Common.hlsli"

StructuredBuffer<Meshlet> Meshlets          : register(t0);
StructuredBuffer<Vertex>  Vertices          : register(t1);
StructuredBuffer<uint>    VertsIndices      : register(t2);
StructuredBuffer<uint>    PrimitiveIndices  : register(t3);

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
  uint gtid : SV_GroupThreadID,
  uint gid : SV_GroupID,
  in  payload PayloadData payload,
  out indices uint3 tris[256],
  out vertices VertexOut verts[128],
  out primitives PrimitiveOut prims[256]
)
{
  Meshlet mesh = Meshlets[0];
  
  float3 offset = payload.Offset[gid];

  SetMeshOutputCounts(mesh.VertsCount, 2 * mesh.PrimsCount);

  if (gtid < mesh.VertsCount) {
    VertexOut vout;
    Vertex v;

    //  32bit index to 16bit index
    uint index = VertsIndices[gtid / 2];
    index = (index >> ((gtid & 0x0001) * 16)) & 0xFFFF;

    v = Vertices[index];
    v.Position *= float3(0.75f, 0.75f, 1);

    vout.Position = float4(v.Position + offset, 1);
    vout.UV = v.UV;
    verts[gtid] = vout;
  }

  if (gtid < mesh.PrimsCount) {
    uint index = PrimitiveIndices[0];
    uint i0 = (index >> 0) & 0xFF;
    uint i1 = (index >> 8) & 0xFF;
    uint i2 = (index >> 16) & 0xFF;
    uint i3 = (index >> 24) & 0xFF;

    tris[gtid * 2 + 0] = uint3(i0, i1, i2);
    tris[gtid * 2 + 1] = uint3(i0, i2, i3);

    PrimitiveOut pout;
    pout.Culling = false;
    prims[gtid * 2 + 0] = pout;

    pout.Culling = false;
    prims[gtid * 2 + 1] = pout;
  }
}
