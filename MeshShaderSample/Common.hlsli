
struct PayloadData 
{
  uint MeshletIndices[32];
};


struct VertexOut
{
  float4  Position : SV_Position;
  float2  UV       : TEXCOORD0;
  uint2 MeshletIndex : COLOR0;
};

struct PrimitiveOut
{
  bool  Culling : SV_CullPrimitive;
};

struct Meshlet
{
  uint    VertsCountAndOffset;  // offset=24bit , count=8bit
  uint    PrimsCountAndOffset;
};

struct Vertex
{
  float3  Position;
  float2  UV;
};

struct Constants
{
  float4x4 World;
  float4x4 WorldView;
  float4x4 WorldViewProj;
  uint     DrawMeshlets;
};

struct MeshInfo
{
  uint MeshletCount;
  uint MeshletOffset;
};
