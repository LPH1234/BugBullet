#pragma once

#include "foundation/PxVec3.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


void glmVec3ToPxVec3(glm::vec3& gv, physx::PxVec3& pv);
physx::PxVec3 glmVec3ToPxVec3(glm::vec3& gv);

void pxVec3ToGlmVec3(physx::PxVec3& pv, glm::vec3& gv);
glm::vec3 pxVec3ToGlmVec3(physx::PxVec3& pv);