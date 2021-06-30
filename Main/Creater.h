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


extern list<PxParticleSystem*> renderParticleSystemList; //储存粒子系统的链表


extern physx::PxRigidDynamic*	airPlane;
extern PlainModel *street;


/**
* @brief			创建一个普通的渲染模型，物理模型是static rigid / dynamic rigid， triangle mesh / convex mesh
* @param pos        模型的初始位置
* @param scale      模型的缩放系数
* @param modelPath  模型文件路径
* @param shader     绘制此模型的shader
* @return			物理模型actor
*/
PxRigidActor* createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic = true);


//碰撞过滤枚举类型
struct FilterGroup
{
	enum Enum
	{
		eBALL = (1 << 0),		//发射的小球
		eWALL = (1 << 1),		//墙壁
		eSTACK = (1 << 2),		//小方块
		eBIGBALL = (1 << 3),	//大球
		ePLAYERBULLET = (1 << 4),	//玩家发射的子弹
		eMISILE = (1 << 5),		//飞机弹药
		eMAP = (1 << 6),		//地图
		eAIRPLANE = (1 << 7),	//飞机
		eTANK = (1 << 8),		//坦克
		eTower=(1<<9),
		ePlayer=(1<<10),
		eTowerBullet = (1<<11)


	};
};


//自定义FilterShader，大球或小球跟方块发生碰撞时为pairFlags添加eCONTACT_DEFAULT
PxFilterFlags testCollisionFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
//测试将Render.cpp中的FilterShader复制过来
PxFilterFlags testCCDFilterShader2(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
void testFilter();

//设置碰撞过滤
void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
//删除removeActorList里面的actor
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

void createPointParticles( //创建普通粒子系统
	int numParticles, //粒子数量
	bool perOffset,  //创建粒子系统的参数，一般是false
	BaseParticle* renderModel, //粒子的渲染模型
	PxVec3 initPos, //粒子初始位置
	bool ifDisperse,//粒子初始化是否散开
	double maxDisperseRadius,//粒子散开的最大半径
	bool ifRandomV, //粒子是否需要随机速度
	double maxRandomV, //粒子最大随机速度（ifRandomV为true时生效）
	int deleteDelaySec = -1, //粒子从产生到被删除的时间（秒）
	int fadeDelaySec = 0, //粒子从产生到开始渐隐的时间（秒）(当deleteDelaySec != -1时生效)
	PxVec3 velocity = PxVec3(0.f, 0.f, 0.f),//粒子的初始速度（hasInitV为true时生效）
	PxVec3 force = PxVec3(0.f, 0.f, 0.f)//粒子系统受到的力
);

void createSmokeParticles( //创建普通粒子系统
	int numParticles, //粒子数量
	bool perOffset,  //创建粒子系统的参数，一般是false
	BaseParticle* renderModel, //粒子的渲染模型
	PxVec3 initPos, //粒子初始位置
	bool ifDisperse,//粒子初始化是否散开
	double maxDisperseRadius,//粒子散开的最大半径
	bool ifRandomV, //粒子是否需要随机速度
	double maxRandomV, //粒子最大随机速度（ifRandomV为true时生效）
	int deleteDelaySec = -1, //粒子从产生到被删除的时间（秒）
	int fadeDelaySec = 0, //粒子从产生到开始渐隐的时间（秒）(当deleteDelaySec != -1时生效)
	PxVec3 velocity = PxVec3(0.f, 0.f, 0.f),//粒子的初始速度（hasInitV为true时生效）
	PxVec3 force = PxVec3(0.f, 0.f, 0.f)//粒子系统受到的力
);

void addForceToPartivleSystem(list<PxParticleSystem*>& particleSystemList);


