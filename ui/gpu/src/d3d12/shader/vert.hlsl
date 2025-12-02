struct VSInput
{
    float2 position : POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = float4(input.position, 0, 1);
    result.color = float4(input.color, 1.0);
    result.texcoord = input.texcoord;

    return result;
}
