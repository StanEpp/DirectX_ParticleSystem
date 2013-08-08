cbuffer cbChangesEveryFrame : register(b0) {
    float3 camPosition;
	matrix viewMatrix;
	float time;
}

cbuffer cbChangesOnResize : register(b1){
	float quadLength;
	matrix projMatrix;
}

struct GS_INPUT{
	float4	pos		: SV_POSITION;
	float2  tex0	: TEXCOORD0;
	float3	color	: COLOR;
};

struct PS_INPUT{
	float4	pos		: SV_POSITION;
	float2  tex0	: TEXCOORD0;
	float3	color	: COLOR;
};

[maxvertexcount(4)]
void GS_Main(point GS_INPUT p[1], inout TriangleStream<PS_INPUT> triStream){
	PS_INPUT p1 = (PS_INPUT)0;
	/*
	float4	viewPos			= float4(p[0].pos);
	viewPos					= mul(viewPos, viewMatrix);
	viewPos					/= viewPos.w;


	float3 v					= viewPos.xyz + float3(1,1,0) * quadLength ;
	float4 vTemp				= {v.xyz, 1.0f};
	p1.pos						= mul(vTemp, projMatrix);
	p1.tex0.x = 1.0f; p1.tex0.y = 0.0f;
	triStream.Append(p1);
	
	v							= viewPos.xyz + float3(1,-1,0) * quadLength ;
	vTemp						= float4(v.xyz, 1.0f);
	p1.pos						= mul(vTemp, projMatrix);
	p1.tex0.x = 1.0f; p1.tex0.y = 1.0f;
	triStream.Append(p1);

	v							= viewPos.xyz + float3(-1,1,0) * quadLength;
	vTemp						= float4(v.xyz, 1.0f);
	p1.pos						= mul(vTemp, projMatrix);
	p1.tex0.x = 0.0f; p1.tex0.y = 0.0f;
	triStream.Append(p1);

	v							= viewPos.xyz + float3(-1,-1,0) * quadLength ;
	vTemp						= float4(v.xyz, 1.0f);
	p1.pos						= mul(vTemp, projMatrix);
	p1.tex0.x = 0.0f; p1.tex0.y = 1.0f;
	triStream.Append(p1);
	*/

    float3 normal			= p[0].pos - camPosition;
	normal					= mul(normal, viewMatrix);

    float3 rightAxis		= cross(float3(0.0f, 1.0f, 0.0f), normal);
    float3 upAxis			= cross(normal, rightAxis);
    rightAxis				= normalize(rightAxis);
    upAxis					= normalize(upAxis);

    float4 rightVector		= float4(rightAxis.xyz, 1.0f);
    float4 upVector         = float4(upAxis.xyz, 1.0f);
	p[0].pos				= mul(p[0].pos, viewMatrix);

    p1.pos = p[0].pos+rightVector*(quadLength)+upVector*(quadLength);
    p1.tex0 = float2(1.0f, 0.0f);
    p1.pos = mul(p1.pos, projMatrix);
	p1.color = p[0].color;
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(quadLength)+upVector*(-quadLength);
    p1.tex0 = float2(1.0f, 1.0f);
    p1.pos = mul(p1.pos, projMatrix);
	p1.color = p[0].color;
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(-quadLength)+upVector*(quadLength);
    p1.tex0 = float2(0.0f, 0.0f);
    p1.pos = mul(p1.pos, projMatrix);
	p1.color = p[0].color;
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(-quadLength)+upVector*(-quadLength);
    p1.tex0 = float2(0.0f, 1.0f);
    p1.pos = mul(p1.pos, projMatrix);
	p1.color = p[0].color;
    triStream.Append(p1);
	


}
