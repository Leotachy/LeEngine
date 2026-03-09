struct VertexData
{
    float x, y, z;
    float r, g, b;
    float u, v;

};

// first param is binding, second is set
[[vk::binding(0, 0)]]
StructuredBuffer<VertexData> in_Vertices : register(t0);

struct Uniforms
{
    float4x4 mvp;
};

// set 0 binding 1
[[vk::binding(1, 0)]]
ConstantBuffer<Uniforms> in_Uniforms;

struct VSOutput
{
    float4 position : SV_Position;
    float3 color;
    float2 texCoords;
};


[shader("vertex")]
VSOutput vertMain(uint vertexIndex : SV_VertexID)
{

    VertexData vtx = in_Vertices[vertexIndex];

    float4 worldPos = float4(vtx.x, vtx.y, vtx.z, 1.0f);
    float4 pos = mul(in_Uniforms.mvp, worldPos);

    VSOutput output;
    output.position = pos;
    output.color = float3(vtx.r, vtx.g, vtx.b);
    output.texCoords = float2(vtx.u, vtx.v);


    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
    return float4(vertIn.color, 1.0);
}