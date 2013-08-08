#include "Particle.h"

Particle::Particle(float x, float y, float z){
	_position = XMFLOAT3(x, y, z);
}

Particle::Particle(){
	_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Particle::~Particle(){}

void Particle::setPosition(XMFLOAT3 &position){
	_position = position;
}

XMFLOAT3 Particle::getPosition(){
	return _position;
}

