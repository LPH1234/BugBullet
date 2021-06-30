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

class BaseSceneObject {
public:
	virtual void oncontact(DATATYPE::ACTOR_TYPE _type) {};
	virtual void oncontact(int id, DATATYPE::ACTOR_TYPE _type) {};
	//生成血条,参数为：被绑定的物体、血条长度、血条位置、joint相对于物体的位置以及joint相对于血条的位置
	/*PxRigidDynamic* createAndShowBlood(PxRigidDynamic* _body, float _healthLength, PxTransform _healthPos, PxTransform t0, PxTransform t1) {
		PxShape* healthShape = gPhysics->createShape(PxBoxGeometry(_healthLength / 2, 0.1f, 0.1f), *gMaterial, true);
		PxRigidDynamic* bloodDynamic = PxCreateDynamic(*gPhysics, _healthPos, *healthShape, 0.0001);
		bloodDynamic->setName("blood");
		bloodDynamic->userData = new UserData(0, "blood", DATATYPE::TRIGGER_TYPE::BLOOD);
		healthShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		healthShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		bloodDynamic->attachShape(*healthShape);
		bloodDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		PxFixedJointCreate(*gPhysics, _body, t0, bloodDynamic, t1);
		return bloodDynamic;
	}*/
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
};
class bonus:public BaseSceneObject {
private:
	int count = 0;
	vector<PxVec3> bonus_pos_list;
	vector<clock_t>timer_list;
	vector<PxRigidStatic* >bonus_list;
	vector<bool> exist_list;
public:
	PxVec3 initbonus(glm::vec3 pos);
	void initlist(vector<glm::vec3>pos_list);
	void autorefresh();

	void runbonus();
};
