
struct PayloadData {
  float3 Offset[8];
};


struct VertexOut
{
  float4  Position : SV_Position;
  float2  UV       : TEXCOORD0;
};

struct PrimitiveOut
{
  bool  Culling : SV_CullPrimitive;
};

struct Meshlet
{
  uint    VertsCount;
  uint    VertsOffset;
  uint    PrimsCount;
  uint    PrimsOffset;
};

struct Vertex
{
  float3  Position;
  float2  UV;
};

