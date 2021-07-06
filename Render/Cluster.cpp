#include "Cluster.h"

BaseParticleCluster::BaseParticleCluster(unsigned int clusterNum, glm::vec3 initPos, Shader* shader) {
	this->initPos = initPos;
	this->shader = shader;
	this->clusterNum = clusterNum;
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


FlameParticleCluster::FlameParticleCluster(unsigned int clusterNum, float flameRadis, float height, float timeToLeave, glm::vec3 initPos, vector<string> textures, Shader* shader) :BaseParticleCluster(clusterNum, initPos, shader) {
	for (int i = 0; i < clusterNum; i++) //int 换成 size_t 报错
	{
		particles.push_back(new FlameParticle(initPos, 300, 20, flameRadis, 0.1f + (i & 1 ? 1 : -1)*i*0.01, height, timeToLeave, "images/textures/flame/flame" + to_string(i) + ".png", shader));
	}
}


SmokeParticleCluster::SmokeParticleCluster(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader) :BaseParticleCluster(clusterNum, initPos, shader) {
	init(clusterNum, smokeDensity, smokeRadis, smokePointSize, smokeVy, smokeMaxY, initPos, textures, shader);

}

SmokeParticleCluster::SmokeParticleCluster(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, string texture, Shader* shader) : BaseParticleCluster(clusterNum, initPos, shader) {
	std::vector<std::string> texs;
	texs.push_back(texture);
	init(clusterNum, smokeDensity, smokeRadis, smokePointSize, smokeVy, smokeMaxY, initPos, texs, shader);
}

void SmokeParticleCluster::init(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader) {
	for (int i = 0; i < clusterNum; i++) //int 换成 size_t 报错
	{
		if (textures.size() == 0)
			particles.push_back(new SmokeParticle(initPos, smokeDensity, smokePointSize, smokeRadis, smokeVy, smokeMaxY, GRAY_SMOKE_TEXTURE_PATH, shader));
		else {
			srand(clock());
			particles.push_back(new SmokeParticle(initPos, smokeDensity, smokePointSize, smokeRadis, smokeVy, smokeMaxY, textures.at(rand() % textures.size()), shader));
		}
	}
}

CloudParticleCluster::CloudParticleCluster(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader) :BaseParticleCluster(clusterNum, initPos, shader) {
	init(clusterNum, cloudDensity, cloudRadis, cloudVy, cloudMaxY, initPos, scale, textures, shader);
}

CloudParticleCluster::CloudParticleCluster(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, string texture, Shader* shader) : BaseParticleCluster(clusterNum, initPos, shader) {
	std::vector<std::string> texs;
	texs.push_back(texture);
	init(clusterNum, cloudDensity, cloudRadis, cloudVy, cloudMaxY, initPos, scale, texs, shader);
}

void CloudParticleCluster::init(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader) {
	this->cloudDensity = cloudDensity;
	for (int i = 0; i < clusterNum; i++) //int 换成 size_t 报错
	{
		if (textures.size() == 0)
			particles.push_back(new CloudParticle(initPos, scale, cloudDensity, cloudRadis, cloudVy, cloudMaxY, GRAY_CLOUD_TEXTURE_PATH, shader));
		else {
			srand(clock());
			particles.push_back(new CloudParticle(initPos, scale, cloudDensity, cloudRadis, cloudVy, cloudMaxY, textures.at(rand() % textures.size()), shader));
		}
	}
}

BoomFlameParticleCluster::BoomFlameParticleCluster(unsigned int clusterNum, float pointSize, float timeToLeave, glm::vec3 initPos, vector<string> textures, Shader* shader) : BaseParticleCluster(clusterNum, initPos, shader) {
	for (int i = 0; i < clusterNum; i++) //int 换成 size_t 报错
	{
		particles.push_back(new BoomFlameParticle(initPos, 300, pointSize, 3.f, timeToLeave, "images/textures/flame/flame" + to_string(i) + ".png", shader));
	}
}