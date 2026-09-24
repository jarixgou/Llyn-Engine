#version 450 core
layout(location = 0) out vec4 fColor;

layout(set=0, binding=0) uniform texture2D _Texture;
layout(set=1, binding=0) uniform sampler _Sampler;

layout(location = 0) in struct {
    vec4 Color;
    vec2 UV;
} In;

vec3 SRGBToLinear(vec3 _sRGB)
{
    return pow(_sRGB, vec3(2.2));
}

void main()
{
    vec4 textureColor = texture(sampler2D(_Texture, _Sampler), In.UV.st);
    fColor = vec4(SRGBToLinear(In.Color.rgb), In.Color.a) * textureColor;
}
