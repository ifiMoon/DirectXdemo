Texture2D Texture : register(t0);
SamplerState MyTextureSampler : register(s0);
static const int bHalfParam = 1;

struct VertexIn
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 PS(VertexOut pIn) : SV_Target
{
    float4 OutColor;
    //OutColor = float4(0.0f, 0.8f, 0.5f, 1.0f);
    //OutColor= Texture.Sample(MyTextureSampler, pIn.uv);
    OutColor= Texture.Sample(MyTextureSampler, float2(pIn.uv.x /2.0, pIn.uv.y / 4.0));
    OutColor = float4(OutColor.r, OutColor.g, OutColor.b, 1.0f);
    return OutColor;
}
 
 