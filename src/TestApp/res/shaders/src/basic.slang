struct VSInput
{
    float3 inPosition;
    float3 inColor;
    float2 inTexCoords;
};

struct UniformBuffer
{
    float4x4 MVP;
};
ConstantBuffer<UniformBuffer> ubo;

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color;
    float2 texCoords;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    VSOutput output;
    output.pos = mul(ubo.MVP, float4(input.inPosition, 1.0));
    output.color = input.inColor;
    output.texCoords = input.inTexCoords;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
    return float4(vertIn.color, 1.0);
}