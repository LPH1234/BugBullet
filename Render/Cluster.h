#pragma once
#include "models.h"
#include <list>

class BaseParticleCluster {
protected:
	glm::vec3 initPos;
	vector<unsigned int> textures;
	Shader* shader;
	list<BaseSpriteParticle*> particles;
	bool readyToLeave = false;
public:
	virtual ~BaseParticleCluster();
	virtual void update() = 0;
	virtual void draw(glm::mat4 view, glm::mat4 projection);
	bool isRemoveable();
};

class FlameParticleCluster : public BaseParticleCluster {
	int flameNum;
public:
	FlameParticleCluster(int flameNum, float flameRadis, float height, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader);
	void update();
};

class SmokeParticleCluster : public BaseParticleCluster {
	int cloudDensity;
public:
	SmokeParticleCluster(int cloudDensity, float cloudRadis, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader);
	void update();
};