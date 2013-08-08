struct VS_INPUT{
	uint vertexID : SV_VertexID;
};

struct GS_INPUT{
	float4	pos		: SV_POSITION;
	float2  tex0	: TEXCOORD0;
};

struct Particle{
	float3 currPos	: POSITION;
	float3 oldPos;	 
};

StructuredBuffer<Particle> particleBuffer : register(u0);

GS_INPUT VS_Main( VS_INPUT input){
	GS_INPUT output = (GS_INPUT)0;

	output.pos.xyz = particleBuffer[input.vertexID].currPos;
	output.pos.w = 1.0f;
	
	output.tex0 = float2(0,0);

	return output;
}