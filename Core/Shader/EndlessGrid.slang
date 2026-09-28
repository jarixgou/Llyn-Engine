struct VSInput
{
    float3 pos;

    float3 normal;
    float3 tangent;
    float3 bitangent;

    float3 color;

    float2 uv0;
    float2 uv1;
}

struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv;
}

[vk::push_constant]
float4x4 VP;

static const float gridSize = 1.0f;

[shader("vertex")]
VSOutput vertMain(VSInput _vsInput)
{
    VSOutput output;

    const float3 scaledPos = mul(_vsInput.pos, gridSize);

    output.pos = mul(float4(scaledPos, 1.0f), VP);
    output.uv = { scaledPos.x, scaledPos.y };

    return output;
}

[shader("fragment")]
float4 fragMain(float4 _pos) : SV_Target
{

}