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
	float4 output = colorMap.Sample(colorSampler, input.tex0)*(8);
	
	float red, green, blue;
	
	red 	= -0.5 + cos(time*0.4+1.5)*0.5;
	green 	= -0.5 + cos(time*0.6)*sin(time*0.3)*0.35;
	blue 	= -0.5 + sin(time*0.2)*0.5;
	
	

	output.x = output.x + red;
	output.y = output.y + green;
	output.z = output.z  + blue;


	return output;
}