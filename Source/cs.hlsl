
cbuffer constantVariables : register(b0){
	uint groupDimX;
	uint groupDimY;
	uint maxParticles;
}

cbuffer dynamicVariables : register(b1){
	float	frameTime;
	float3	attractor;
}

struct Particle{
	float3 currPos;
	float3 oldPos;
};

RWStructuredBuffer<Particle> srcParticleBuffer : register(u0);

[numthreads(1024, 1, 1)]
void CS_Main( uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex  )
{
		//Calculate the ID of this Thread
		uint ID = groupID.x * 1024 + groupID.y * groupDimX * 1024 + groupID.z * groupDimX * groupDimY * 1024 + groupIndex; 
		
		//Every thread renders a particle. 
		//If there are more threads than particles then stop here.
		if(ID < maxParticles){
			Particle p = srcParticleBuffer[ID];
		
			float3 a;
			float3 tempCurrPos;

			if(attractor.x != 0 && attractor.y != 0 && attractor.z != 0){
				a = attractor - p.currPos;
				a = normalize(a)*500;
			} else {
				a = float3(0,-1,0);
			}

			tempCurrPos = 2.0*p.currPos - p.oldPos + a*frameTime*frameTime;
			p.oldPos	= p.currPos;
			p.currPos	= tempCurrPos;
			
			//Keep all the particles inside
			if(length(p.currPos) > 55){
				float3 norm = normalize(p.currPos);
				p.currPos = norm*55;
			}

			srcParticleBuffer[ID] = p;
		}
}