struct PS_INPUT
{
    float4 outposition  : SV_POSITION;
    float4 color        : COLOR_;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return input.color;
}