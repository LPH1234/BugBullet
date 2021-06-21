#include"Convert.h"


void glmVec3ToPxVec3(glm::vec3& gv, physx::PxVec3& pv) {
	pv.x = gv.x;
	pv.y = gv.y;
	pv.z = gv.z;
}

physx::PxVec3 glmVec3ToPxVec3(glm::vec3& gv) {
	physx::PxVec3 pv;
	glmVec3ToPxVec3(gv, pv);
	return pv;
}


void pxVec3ToGlmVec3(physx::PxVec3& pv, glm::vec3& gv) {
	gv.x = pv.x;
	gv.y = pv.y;
	gv.z = pv.z;
}
glm::vec3 pxVec3ToGlmVec3(physx::PxVec3& pv) {
	glm::vec3 gv;
	pxVec3ToGlmVec3(pv, gv);
	return gv;
}


float getVec3Length(glm::vec3& v) {
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}
float getVec3Length(physx::PxVec3& v) {
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}