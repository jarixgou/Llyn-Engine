[vk::binding(0, 0)]
Texture2D<float4> srcTexture;

static const float2 vertices[6] =
{
    { -1.0, -1.0 }, { 1.0, -1.0 }, { 1.0, 1.0 },
    { -1.0, -1.0 }, { 1.0, 1.0 },  { -1.0, 1.0 }
};

[shader("vertex")]
float4 vertMain(uint _vertexID : SV_VertexID) : SV_Position
{
    float2 vertex = vertices[_vertexID];

    return float4(vertex, 0, 1.0);
}

[shader("fragment")]
float4 fragMain(float4 _pos : SV_Position) : SV_Target
{
    return srcTexture.Load(int3(int2(_pos.xy), 0));
}