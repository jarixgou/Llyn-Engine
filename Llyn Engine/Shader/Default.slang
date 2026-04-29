struct VSInput
{
    float3 position;
    float3 normal;
    float3 color;
    float2 texCoord; 
}

struct VSOutput
{
    float4 vsPosition : SV_Position;

    float3 fragPosition;
    float3 normal;
    float3 color;
    float2 texCoord;
}

[shader("vertex")]
VSOutput vertMain(VSInput _input)
{
    VSOutput output;

    output.color = _input.color;
    output.normal = _input.normal;

    //output.vsPosition = proj * view * float4(_input.fragPosition)
    output.vsPosition = float4(_input.position, 1.0);
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput _inVert) : SV_Target
{
    float3 color = _inVert.color;
    return float4(color, 1.0);
}