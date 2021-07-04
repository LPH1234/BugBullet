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
#include "Creater.h"

//extern struct DATATYPE;
extern PxPhysics*				gPhysics;
extern PxScene*					gScene;
extern PxMaterial*				gMaterial;
extern PxRigidDynamic* createCollection(PxTransform &tran, DATATYPE::TRIGGER_TYPE _type);
extern vector<PxTransform> addBonusList;
class BaseSceneObject {
public:
	virtual void oncontact(DATATYPE::ACTOR_TYPE _type) {};
	virtual void oncontact(int id, DATATYPE::ACTOR_TYPE _type) {};
	virtual bool supplyoncontact(int id, DATATYPE::ACTOR_TYPE _type) { return false; };
};
class guntower : public BaseSceneObject
{
public:
	int count = 0;
	PxVec3 towerpos;
	clock_t timer_last = 0;
	vector<PxVec3> towerpos_list;
	vector<clock_t>timer_list;
	vector<PxRigidStatic* >tower_list;
	PxVec3 headforward = PxVec3(1.0f, 0.0f, 0.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 currentheadforward;
	PxVec3 currentbackforward;
	
	vector<int>health_list;	
	vector<bool>enable_attack_list;
	vector<PxRigidStatic*>blood_body_list;

	PxVec3 initguntower(glm::vec3 pos);
	PxQuat getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront);
	void fire(const PxTransform& t, const PxVec3& velocity);
	void autoattack(PxRigidDynamic* target, PxVec3 pos);
	void rotate(PxRigidDynamic* target, PxVec3 current);
	void runguntower(PxRigidDynamic* target);
	void initlist(vector<glm::vec3> pos_list);
	void oncontact(int id, DATATYPE::ACTOR_TYPE _type);
	void reset();
};
class bonus:public BaseSceneObject {
private:
	int count = 0;
	vector<PxVec3> supply_pos_list;
	vector<clock_t>timer_list;
	//vector<PxRigidStatic* >bonus_list;
	vector<bool> enable_supply_list;
	vector<PxRigidDynamic*>supply_list;
	bool odd=false;//交替生成补血和补弹药
public:
	PxVec3 initsupply(glm::vec3 pos);
	void initlist(vector<glm::vec3>pos_list);
	void autorefresh(int id);
	void runsupply();
	static void generate_bonus_pos(PxTransform &t) {
		/*PxRigidDynamic* bonus = reinterpret_cast<PxRigidDynamic*>(createCollection(t, DATATYPE::TRIGGER_TYPE::COLLECTION));
		bonus->userData = new UserData(0, "BONUS", DATATYPE::TRIGGER_TYPE::COLLECTION);
		bonus->setName("BONUS");*/
		addBonusList.push_back(t);
	};
	bool supplyoncontact(int id, DATATYPE::ACTOR_TYPE _type);
	void reset();
};
