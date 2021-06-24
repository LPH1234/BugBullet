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


namespace Snippets
{
	void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, Shader* shader, bool shadows = false, const physx::PxVec3 & color = physx::PxVec3(0.0f, 0.75f, 0.0f));
	void renderParticles(list<PxParticleSystem*>& particleSystemList, Shader* shader);
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

#endif //PHYSX_SNIPPET_RENDER_H
