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
#include "SceneObjects.h"
#include <list>
#include <cmath>
#include <ctime>
#include "Characters.h"


//extern set<PxRigidDynamic*>		airPlaneBullet;
extern PxFoundation*			gFoundation;
extern PxPhysics*				gPhysics;
extern PxCooking*				gCooking;
extern PxDefaultCpuDispatcher*	gDispatcher;
extern PxScene*				gScene;
extern PxMaterial*				gMaterial;
extern PxPvd*                  gPvd;


extern list<PxParticleSystem*> renderParticleSystemList; //��������ϵͳ������


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
		eMISILE = (1 << 5),		//�ɻ���ҩ
		eMAP = (1 << 6),		//��ͼ
		eAIRPLANE = (1 << 7),	//�ɻ�
		eTANK = (1 << 8),		//̹��
		eTower=(1<<9),
		ePlayer=(1<<10),
		eTowerBullet = (1<<11)


	};
};


//�Զ���FilterShader�������С������鷢����ײʱΪpairFlags���eCONTACT_DEFAULT
PxFilterFlags testCollisionFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
//���Խ�Render.cpp�е�FilterShader���ƹ���
PxFilterFlags testCCDFilterShader2(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
void testFilter();

//������ײ����
void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
//ɾ��removeActorList�����actor
void removeActorInList();

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(20));

PxRigidDynamic* init3rdplayer(const PxTransform& t, const PxGeometry& geometry);

//PxRigidDynamic* initvehicle(const PxTransform& t, const PxGeometry& geometry);



void createBigBall();
void createAirPlane();
void changeAirPlaneVelocity();
double getAngel(PxVec3 a, PxVec3 b);
bool lessThan180(PxVec3 a, PxVec3 base);
void createAbleBreakWall();
void createBreakableWall();
void testTriggerWall();
void testTriggerCollection();
PxRigidDynamic* createCollection(PxTransform &tran, DATATYPE::TRIGGER_TYPE _type);


void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);

void createBullet(const PxTransform& t, const PxVec3& velocity);

void createPointParticles( //������ͨ����ϵͳ
	int numParticles, //��������
	bool perOffset,  //��������ϵͳ�Ĳ�����һ����false
	BaseParticle* renderModel, //���ӵ���Ⱦģ��
	PxVec3 initPos, //���ӳ�ʼλ��
	bool ifDisperse,//���ӳ�ʼ���Ƿ�ɢ��
	double maxDisperseRadius,//����ɢ�������뾶
	bool ifRandomV, //�����Ƿ���Ҫ����ٶ�
	double maxRandomV, //�����������ٶȣ�ifRandomVΪtrueʱ��Ч��
	int deleteDelaySec = -1, //���ӴӲ�������ɾ����ʱ�䣨�룩
	int fadeDelaySec = 0, //���ӴӲ�������ʼ������ʱ�䣨�룩(��deleteDelaySec != -1ʱ��Ч)
	PxVec3 velocity = PxVec3(0.f, 0.f, 0.f),//���ӵĳ�ʼ�ٶȣ�hasInitVΪtrueʱ��Ч��
	PxVec3 force = PxVec3(0.f, 0.f, 0.f)//����ϵͳ�ܵ�����
);

void createSmokeParticles( //������ͨ����ϵͳ
	int numParticles, //��������
	bool perOffset,  //��������ϵͳ�Ĳ�����һ����false
	BaseParticle* renderModel, //���ӵ���Ⱦģ��
	PxVec3 initPos, //���ӳ�ʼλ��
	bool ifDisperse,//���ӳ�ʼ���Ƿ�ɢ��
	double maxDisperseRadius,//����ɢ�������뾶
	bool ifRandomV, //�����Ƿ���Ҫ����ٶ�
	double maxRandomV, //�����������ٶȣ�ifRandomVΪtrueʱ��Ч��
	int deleteDelaySec = -1, //���ӴӲ�������ɾ����ʱ�䣨�룩
	int fadeDelaySec = 0, //���ӴӲ�������ʼ������ʱ�䣨�룩(��deleteDelaySec != -1ʱ��Ч)
	PxVec3 velocity = PxVec3(0.f, 0.f, 0.f),//���ӵĳ�ʼ�ٶȣ�hasInitVΪtrueʱ��Ч��
	PxVec3 force = PxVec3(0.f, 0.f, 0.f)//����ϵͳ�ܵ�����
);

void addForceToPartivleSystem(list<PxParticleSystem*>& particleSystemList);


