

#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 ViewProjMatrix;
} globalUniformBuffer;

layout(set = 1, binding = 0) uniform MeshUniformBuffer
{
    mat4 ModelMatrix;
} meshUniformBuffer;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoords;

void main()
{
    mat4 MvpMatrix =
        globalUniformBuffer.ViewProjMatrix *
        meshUniformBuffer.ModelMatrix;

    gl_Position = MvpMatrix * vec4(inPosition, 1.0);

    fragColor = inColor;
    fragTexCoords = inTexCoords;
}