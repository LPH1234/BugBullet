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
	vector<TowerData* >tower_list;
public:
	PxVec3 initguntower(glm::vec3 pos);
	void autoattack(PxRigidDynamic* target, PxVec3 pos);
	void runguntower(PxRigidDynamic* target);
	void initlist(vector<glm::vec3> pos_list);
};

