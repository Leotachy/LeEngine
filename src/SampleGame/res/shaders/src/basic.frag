

#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D baseTexture;

layout( push_constant ) uniform constants
{
    mat4 worldTM;
    vec4 materialColor;

} PushConstants;

void main()
{
    vec3 texColor = texture(baseTexture, fragTexCoords).rgb;

    outColor = PushConstants.materialColor * vec4(fragColor * texColor, 1.0);
}