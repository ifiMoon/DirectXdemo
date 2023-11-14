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

VertexOut VS(VertexIn vIn)
{
    VertexOut vOut;
    vOut.posH = float4(vIn.pos, 1.0f);
    vOut.uv = vIn.uv;
    return vOut;
}
