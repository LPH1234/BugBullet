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


extern PxFoundation*			gFoundation;
extern PxPhysics*				gPhysics;
extern PxCooking*				gCooking;
extern PxDefaultCpuDispatcher*	gDispatcher;
extern PxScene*				gScene;
extern PxMaterial*				gMaterial;
extern PxPvd*                  gPvd;

extern physx::PxRigidDynamic* player;
extern physx::PxRigidDynamic* vehicle;

extern list<PxParticleSystem*> renderParticleSystemList; //��������ϵͳ������
//����
extern physx::PxRigidDynamic* guntower_1;
extern physx::PxRigidDynamic* guntower_2;
extern physx::PxRigidDynamic* guntower_3;
extern physx::PxRigidDynamic* guntower_4;

extern physx::PxRigidDynamic*	airPlane;
extern PlainModel *street;


/**
* @brief			����һ����ͨ����Ⱦģ�ͣ�����ģ����static rigid / dynamic rigid�� triangle mesh / convex mesh
* @param pos        ģ�͵ĳ�ʼλ��
* @param scale      ģ�͵�����ϵ��
* @param modelPath  ģ���ļ�·��
* @param shader     ���ƴ�ģ�͵�shader
* @return			����ģ��actor
*/
PxRigidActor* createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic = true);


//��ײ����ö������
struct FilterGroup
{
	enum Enum
	{
		eBALL = (1 << 0),		//�����С��
		eWALL = (1 << 1),		//ǽ��
		eSTACK = (1 << 2),		//С����
		eBIGBALL = (1 << 3),	//����
		ePLAYERBULLET = (1 << 4),	//��ҷ�����ӵ�
	};
};


//�Զ���FilterShader�������С������鷢����ײʱΪpairFlags���eCONTACT_DEFAULT
PxFilterFlags testCollisionFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

//������ײ����
void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
//ɾ��removeActorList�����actor
void removeActorInList();

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));

PxRigidDynamic* init3rdplayer(const PxTransform& t, const PxGeometry& geometry);

PxRigidDynamic* initvehicle(const PxTransform& t, const PxGeometry& geometry);



void createBigBall();
void createAirPlane();
void changeAirPlaneVelocity();
double getAngel(PxVec3 a, PxVec3 b);
bool lessThan180(PxVec3 a, PxVec3 base);
void createAbleBreakWall();


void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);

void createBullet(const PxTransform& t, const PxVec3& velocity);

void createParticles(int numParticles, bool perOffset, PxVec3 initPos, PxVec3 velocity = PxVec3(0.f, 0.f, 0.f), PxVec3 force = PxVec3(0.f, 0.f, 0.f));


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
