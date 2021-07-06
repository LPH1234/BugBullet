#pragma once
#include "models.h"
#include <list>

class BaseParticleCluster {
protected:
	glm::vec3 initPos;
	Shader* shader;
	list<BaseSpriteParticle*> particles;
	bool readyToLeave = false;
	unsigned int clusterNum;  //具体的粒子簇数量
	int density;  //粒子顶点数量
public:
	BaseParticleCluster(unsigned int clusterNum, glm::vec3 initPos, Shader* shader);
	virtual ~BaseParticleCluster();
	virtual void update();
	virtual void draw(glm::mat4 view, glm::mat4 projection);
	bool isRemoveable();
};

class FlameParticleCluster : public BaseParticleCluster {
public:
	FlameParticleCluster(unsigned int clusterNum, float flameRadis, float height, float timeToLeave, glm::vec3 initPos, vector<string> textures, Shader* shader);
};

class SmokeParticleCluster : public BaseParticleCluster {
	int smokeDensity;
	void init(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader);
public:
	SmokeParticleCluster(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader);
	SmokeParticleCluster(unsigned int clusterNum, int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, string texture, Shader* shader);
};

class CloudParticleCluster : public BaseParticleCluster {
	int cloudDensity;
	void init(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader);
public:
	CloudParticleCluster(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader);
	CloudParticleCluster(unsigned int clusterNum, int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, string texture, Shader* shader);
};

class BoomFlameParticleCluster : public BaseParticleCluster {
public:
	BoomFlameParticleCluster(unsigned int clusterNum, float pointSize, float timeToLeave, glm::vec3 initPos, vector<string> textures, Shader* shader);
};