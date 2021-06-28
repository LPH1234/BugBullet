#pragma once
#include "PxPhysicsAPI.h"
#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include "../Render/objLoader.h"
#include "../Utils/module.h"
#include "../Render/models.h"
#include "../Render/Render.h"
#include "../Data/Data.h"
#include "../Data/Consts.h"

#include <list>
#include <cmath>
#include <ctime>

class guntower
{
private:
	PxVec3 towerpos;
	clock_t timer_last = 0;
	vector<PxVec3> towerpos_list;
	vector<clock_t>timer_list;
	vector<PxRigidStatic* >tower_list;
	PxVec3 headforward = PxVec3(1.0f, 0.0f, 0.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 currentheadforward;
	PxVec3 currentbackforward;
public:
	PxVec3 initguntower(glm::vec3 pos);
	void autoattack(PxRigidDynamic* target, PxVec3 pos);
	void rotate(PxRigidDynamic* target, PxVec3 current);
	void runguntower(PxRigidDynamic* target);
	void initlist(vector<glm::vec3> pos_list);
};
class bonus {
private:

public:
	PxVec3 initbonus(glm::vec3 pos);
	void initlist(vector<glm::vec3>pos_list);
	void runbonus(PxRigidDynamic* target);
};
