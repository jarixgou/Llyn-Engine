struct VSInput
{
    float3 pos;
    float3 normal;
    float3 color;
    float2 texCoord; 
}

struct VSOutput
{
    float4 vsPosition : SV_Position;

    float3 fragPos;
    float3 normal;
    float3 color;
    float2 texCoord;
}

struct ModelData
{
    float4x4 model;
    float4x4 invModel;
}

struct Camera
{
    float4x4 view;
    float4x4 proj;

    float3 pos;
}

[[vk::binding(0, 0)]]
ConstantBuffer<Camera> camera;

[[vk::binding(1, 0)]]
ConstantBuffer<ModelData> modelData;

[shader("vertex")]
VSOutput vertMain(VSInput _input)
{
    VSOutput output;

    output.color = _input.color;
    output.texCoord = _input.texCoord;

    output.fragPos = mul(modelData.model, float4(_input.pos, 1.0f)).xyz;
    //output.fragPosition = _input.position;

    // output.normal = transpose(inverse(mat3(model))) * _input.normal;
    output.normal = mul((float3x3)modelData.invModel, _input.normal);

    // output.vsPosition = proj * view * float4(_input.fragPosition, 1.0);
    output.vsPosition = mul(camera.proj, mul(camera.view, float4(output.fragPos, 1.0)));
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput _outVS) : SV_Target
{
    float3 color = _outVS.color;
    return float4(color, 1.0);
}