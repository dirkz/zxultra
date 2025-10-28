cbuffer cbProjection : register(b0)
{
    float4x4 Projection;
}

cbuffer cbView : register(b1)
{
    float4x4 View;
}

cbuffer cbWorld : register(b2)
{
    float4x4 World;
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
    float4 position = mul(hPosition, World);
    position = mul(position, View);
    position = mul(position, Projection);
    fragment.Position = position;
    fragment.Color = v.Color;
    
    return fragment;
}

float4 PS(Fragment fragment) : SV_Target
{
    return fragment.Color;
}