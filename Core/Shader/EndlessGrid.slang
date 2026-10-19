struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv;
}

[vk::push_constant]
float4x4 VP;

static const float3 vertices[4] = 
{
    { -0.5, 0.0, 0.5 },
    { 0.5, 0.0, 0.5 },
    { 0.5, 0.0, -0.5 },
    { -0.5, 0.0, -0.5 },
};

static const float gridSize = 1.0f;

[shader("vertex")]
VSOutput vertMain(uint32_t _vertexID : SV_VertexID)
{
    VSOutput output;

    const float3 vertex = vertices[_vertexID];

    const float3 scaledPos = mul(vertex, gridSize);

    output.pos = mul(float4(scaledPos, 1.0f), VP);
    output.uv = { scaledPos.x, scaledPos.z };

    return output;
}

[shader("fragment")]
float4 fragMain(float4 _pos) : SV_Target
{
    return float4(0.0);
}