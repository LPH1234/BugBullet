#include "Cluster.h"

void BaseParticleCluster::draw(glm::mat4 view, glm::mat4 projection) {
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	list<BaseSpriteParticle*>::iterator it;
	bool removeFlag = true;
	for (it = particles.begin(); it != particles.end(); it++) {
		(*it)->draw();
		if (!(*it)->isRemoveable())
			removeFlag = false;
	}
	this->readyToLeave = removeFlag;
}

BaseParticleCluster::~BaseParticleCluster() {
	list<BaseSpriteParticle*>::iterator it;
	for (it = particles.begin(); it != particles.end(); it++) {
		BaseSpriteParticle* p = (*it);
		delete p;
	}
	particles.clear();
}

bool BaseParticleCluster::isRemoveable() {
	return this->readyToLeave;
}


FlameParticleCluster::FlameParticleCluster(int flameNum, float flameRadis, float height, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader) {
	this->flameNum = flameNum;
	this->initPos = initPos;
	this->textures = textures;
	this->shader = shader;
	for (int i = 0; i < flameNum; i++) //int »»³É size_t ±¨´í
	{
		particles.push_back(new FlameParticle(initPos, 600, 20, flameRadis, 0.1f + (i&1?1:-1)*i*0.01, height, "images/textures/flame/flame"+ to_string(i) + ".png", shader));
	}

}

void  FlameParticleCluster::update() {

}

SmokeParticleCluster::SmokeParticleCluster(int cloudDensity, float cloudRadis, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader){
	this->cloudDensity = cloudDensity;
	this->initPos = initPos;
	this->textures = textures;
	this->shader = shader;
	particles.push_back(new SmokeParticle(initPos, cloudDensity, 30, cloudRadis, 0.01f, 1.4f, "images/textures/smoke/smoke1.png", shader));
}


void  SmokeParticleCluster::update() {

}
