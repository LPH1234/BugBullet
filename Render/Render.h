#pragma once

#ifndef PHYSX_SNIPPET_RENDER_H
#define PHYSX_SNIPPET_RENDER_H



#include <iostream>
#include<string>
#include <windows.h>
#pragma warning(disable: 4505)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Utils/Convert.h"
#include "Model.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#include "objLoader.h"
#include "Cluster.h"
#include "UI.h"


namespace Render
{
	void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, Shader* shader, bool shadows = false, const physx::PxVec3 & color = physx::PxVec3(0.0f, 0.75f, 0.0f));
	void renderParticles(list<PxParticleSystem*>& particleSystemList, list<BaseParticleCluster*>& renderParticleClusterList, glm::mat4 view, glm::mat4 projection);
	//void renderUI(glm::mat4& projection, list<BaseUI*>& UIList);
}

PxFilterFlags testCCDFilterShader(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize);

#define MAX_NUM_ACTOR_SHAPES 128

class ParticleSystemData {
public:
	short id;
	bool hasForce;
	int numParticles;
	int createTime; //�����������ϵͳʱ��clock()ʱ���
	int deleteDelaySec; //�������������ʱ������ϵͳӦ�ñ��Ƴ�
	int fadeDelaySec;//�����������ʱ����Ⱦģ�Ϳ�ʼ����(fadeDelaySec < fadeSec)
	BaseParticle* renderModel = nullptr;
	PxVec3 *newAppParticleforces = nullptr;
	PxU32 *newAppParticleIndices = nullptr;
	PxParticleExt::IndexPool* indexPool = nullptr;
	PxVec4* axisAndAngle = nullptr;
	~ParticleSystemData() {
		if (this->renderModel != nullptr)
			delete this->renderModel;
		if (this->newAppParticleforces != nullptr)
			delete this->newAppParticleforces;
		if (this->newAppParticleIndices != nullptr)
			delete this->newAppParticleIndices;
		if (this->axisAndAngle != nullptr)
			delete this->axisAndAngle;
		if (this->indexPool != nullptr) {
			indexPool->freeIndices();
			indexPool->release();
		}

	}
};


#endif //PHYSX_SNIPPET_RENDER_H
