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




extern PxFoundation*			gFoundation;
extern PxPhysics*				gPhysics;
extern PxCooking*				gCooking;
extern PxDefaultCpuDispatcher*	gDispatcher;
extern PxScene*				gScene;
extern PxMaterial*				gMaterial;
extern PxPvd*                  gPvd;

extern physx::PxRigidDynamic* player;
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
	};
};


//自定义FilterShader，大球或小球跟方块发生碰撞时为pairFlags添加eCONTACT_DEFAULT
PxFilterFlags testCollisionFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

//设置碰撞过滤
void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
//删除removeActorList里面的actor
void removeActorInList();

PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));

PxRigidDynamic* init3rdplayer(const PxTransform& t, const PxGeometry& geometry);

void createBigBall();
void createAirPlane();
void changeAirPlaneVelocity();

PxRigidDynamic* initPlayer();


void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);

void createBullet(const PxTransform& t, const PxVec3& velocity);