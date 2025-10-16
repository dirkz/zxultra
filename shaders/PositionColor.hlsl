cbuffer cbPerObject : register(b0)
{
    float4x4 WorldViewProjection;
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

    float4 hPosition = float4(v.Position, 1);
    fragment.Position = mul(hPosition, WorldViewProjection);
    fragment.Color = v.Color;
    
    return fragment;
}

float4 PS(Fragment fragment)
{
    return fragment.Color;
}