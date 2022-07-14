struct VS_INPUT
{
    float2 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 outposition  : SV_POSITION;
    float4 color        : COLOR_;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outposition = float4(input.position, 0.5f, 1.0f);
    output.color       = input.color;
    return output;
}