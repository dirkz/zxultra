cbuffer cbModel : register(b0)
{
    float4x4 Model;
}

cbuffer cbViewProjection : register(b1)
{
    float4x4 ViewProjection;
}

struct Vertex
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct Fragment
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

Fragment VS(Vertex v)
{
    Fragment fragment;

    float4x4 transform = mul(Model, ViewProjection);

    float4 hPosition = float4(v.Position, 1);
    float4 position = mul(hPosition, transform);

    fragment.Position = position;
    fragment.Color = v.Color;
    
    return fragment;
}

float4 PS(Fragment fragment) : SV_Target
{
    return fragment.Color;
}