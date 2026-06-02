

#version 450

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoords;

layout(set = 0, binding = 0) uniform GlobalUniformBuffer
{
    mat4 ViewProjMatrix;
} globalUniformBuffer;

layout( push_constant ) uniform constants
{
    mat4 worldTM;
    vec4 materialColor;

} PushConstants;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoords;

void main()
{
    mat4 MvpMatrix =
        globalUniformBuffer.ViewProjMatrix *
        PushConstants.worldTM;

    gl_Position = MvpMatrix * vec4(vertexPos, 1.0);

    fragColor = vertexColor;
    fragTexCoords = vertexTexCoords;
}