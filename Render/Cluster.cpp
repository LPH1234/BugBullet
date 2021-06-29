#include "Cluster.h"

BaseParticleCluster::BaseParticleCluster(glm::vec3 initPos, vector<string> textures, Shader* shader) {
	this->initPos = initPos;
	this->textures = textures;
	this->shader = shader;
}

BaseParticleCluster::~BaseParticleCluster() {
	list<BaseSpriteParticle*>::iterator it;
	for (it = particles.begin(); it != particles.end(); it++) {
		BaseSpriteParticle* p = (*it);
		delete p;
	}
	particles.clear();
}

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

void  BaseParticleCluster::update() {

}


bool BaseParticleCluster::isRemoveable() {
	return this->readyToLeave;
}


FlameParticleCluster::FlameParticleCluster(int flameNum, float flameRadis, float height, glm::vec3 initPos, vector<string> textures, Shader* shader) :BaseParticleCluster(initPos, textures, shader) {
	this->flameNum = flameNum;
	for (int i = 0; i < flameNum; i++) //int »»³É size_t ±¨´í
	{
		particles.push_back(new FlameParticle(initPos, 600, 20, flameRadis, 0.1f + (i & 1 ? 1 : -1)*i*0.01, height, "images/textures/flame/flame" + to_string(i) + ".png", shader));
	}
}


SmokeParticleCluster::SmokeParticleCluster(int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader) :BaseParticleCluster(initPos, textures, shader) {
	particles.push_back(new SmokeParticle(initPos, smokeDensity, smokePointSize, smokeRadis, smokeVy, smokeMaxY, "images/textures/smoke/smoke1.png", shader));
}



CloudParticleCluster::CloudParticleCluster(int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader) :BaseParticleCluster(initPos, textures, shader) {
	this->cloudDensity = cloudDensity;
	particles.push_back(new CloudParticle(initPos, scale, cloudDensity, cloudRadis, cloudVy, cloudMaxY, "images/textures/smoke/smoke-gray-0.png", shader));
}
