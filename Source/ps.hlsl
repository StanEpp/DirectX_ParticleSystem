Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

cbuffer timer : register(b0){
	float time;
}

struct PS_INPUT{
	float4	pos		: SV_POSITION;
	float2  tex0	: TEXCOORD0;
};

float4 PS_Main(PS_INPUT input) : SV_TARGET{
	float4 output = colorMap.Sample(colorSampler, input.tex0)*(10);

	output.x = output.x - 0.4 + cos(0.1*time)*0.4;
	output.y = output.y - 0.6 + cos(0.6*time)*0.6;
	output.z = output.z  - 0.4 + sin(0.3*time)*0.4;


	return output;
}