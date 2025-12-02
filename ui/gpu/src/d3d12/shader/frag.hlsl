struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
  return input.color;
}
