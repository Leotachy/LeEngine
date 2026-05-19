struct VSInput
{
    float3 inPosition;
    float3 inColor;
    float2 inTexCoords;
};

struct GlobalUniformBuffer
{
    float4x4 ViewProjMatrix;
};
[[vk::binding(0, 0)]]
ConstantBuffer<GlobalUniformBuffer> globalUniformBuffer;

struct MeshUniformBuffer
{
    float4x4 ModelMatrix;
};
[[vk::binding(0, 1)]]
ConstantBuffer<MeshUniformBuffer> meshUniformBuffer;

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
    float2 texCoords;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{

    float4x4 MvpMatrix = mul(globalUniformBuffer.ViewProjMatrix, meshUniformBuffer.ModelMatrix);

    VSOutput output;
    output.pos = mul(MvpMatrix, float4(input.inPosition, 1.0));
    output.color = input.inColor;
    output.texCoords = input.inTexCoords;
    return output;
}

[[vk::binding(0, 2)]]
Sampler2D baseTexture;

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
    return float4(vertIn.color * baseTexture.Sample(vertIn.texCoords).rgb, 1.0);
}