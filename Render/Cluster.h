#pragma once
#include "models.h"
#include <list>

class BaseParticleCluster {
protected:
	glm::vec3 initPos;
	vector<string> textures;
	Shader* shader;
	list<BaseSpriteParticle*> particles;
	bool readyToLeave = false;
	int density;  //粒子密度
	int num;  //粒子数量
public:
	BaseParticleCluster(glm::vec3 initPos, vector<string> textures, Shader* shader);
	virtual ~BaseParticleCluster();
	virtual void update();
	virtual void draw(glm::mat4 view, glm::mat4 projection);
	bool isRemoveable();
};

class FlameParticleCluster : public BaseParticleCluster {
	int flameNum;
public:
	FlameParticleCluster(int flameNum, float flameRadis, float height, glm::vec3 initPos, vector<string> textures, Shader* shader);
};

class SmokeParticleCluster : public BaseParticleCluster {
	int smokeDensity;
public:
	SmokeParticleCluster(int smokeDensity, float smokeRadis, float smokePointSize, float smokeVy, float smokeMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader);
};

class CloudParticleCluster : public BaseParticleCluster {
	int cloudDensity;
public:
	CloudParticleCluster(int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, glm::vec3 scale, vector<string> textures, Shader* shader);
};