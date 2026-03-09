struct VertexData
{
    float x, y, z;
    float r, g, b;
    float u, v;

};

// first param is set, second is binding
[[vk::binding(0, 0)]]
StructuredBuffer<VertexData> in_Vertices : register(t0);

struct VSOutput
{
    float4 position : SV_Position;
    float3 color;
    float2 texCoords;
};


[shader("vertex")]
VSOutput vertMain(uint vertexIndex : SV_VertexID)
{
    VSOutput output;

    VertexData vtx = in_Vertices[vertexIndex];

    float3 pos = float3(vtx.x, vtx.y, vtx.z);

    output.position = float4(pos, 1.0);
    output.color = float3(vtx.r, vtx.g, vtx.b);
    output.texCoords = float2(vtx.u, vtx.v);


    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET
{
    return float4(vertIn.color, 1.0);
}