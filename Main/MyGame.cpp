#include <ctype.h>
#include <iostream>
#include <ctime>

#include "Creater.h"

#define PI 3.1415926

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
PxSimulationEventCallback *myCallBack;
PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics = NULL;
PxCooking*				gCooking = NULL;
PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene = NULL;
PxMaterial*				gMaterial = NULL;
PxPvd*                  gPvd = NULL;

PxReal					stackZ = 3.0f;
vector<PxActor*>		removeActorList;
clock_t					lockFrame_last = 0, lockFrame_current = 0;

//切换射击机制
bool autoshooting = true;
clock_t last = 0;

//
PxRigidDynamic* player_ctl=NULL;
PxVec3					airPlaneVelocity(0, 0, 0);
PxRigidDynamic*			airPlane = NULL;
long long				angelAirPlane = 0.0;
PxVec3					headForward(1, 0, 0);

//碰撞过滤枚举类型
struct FilterGroup
{
	enum Enum
	{
		eBALL = (1 << 0),		//发射的小球
		eWALL = (1 << 1),		//墙壁
		eSTACK = (1 << 2),		//小方块
		eBIGBALL = (1 << 3),	//大球
	};
};

/**
* @brief			创建一个普通的渲染模型，物理模型是static rigid / dynamic rigid， triangle mesh / convex mesh
* @param pos        模型的初始位置
* @param scale      模型的缩放系数
* @param modelPath  模型文件路径
* @param shader     绘制此模型的shader
* @return			是否成功
*/
bool createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic = true);
/**
* @brief			根据一个自定义的渲染模型去创建物理模型，物理模型是static rigid / dynamic rigid， triangle mesh / convex mesh
* @param model      指向渲染模型的指针
* @return			是否成功
*/
bool createSpecialStaticModel(BaseModel* model, bool preLoad = false, bool ifStatic = true);

//自定义FilterShader，大球或小球跟方块发生碰撞时为pairFlags添加eCONTACT_DEFAULT
PxFilterFlags testCollisionFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 == filterData1.word1) || (filterData1.word0 == filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;

}

//设置碰撞过滤
void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*)*numShapes);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	free(shapes);
}
//碰撞回调函数
void module::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	//PX_UNUSED((pairHeader));
	//printf("Enter onContact!\n");
	std::vector<PxContactPairPoint> contactPoints;//存储每一个触碰点信息
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxRigidActor* actor_0 = (PxRigidActor*)(pairHeader.actors[0]);
		PxRigidActor* actor_1 = (PxRigidActor*)(pairHeader.actors[1]);
		if (actor_0 != NULL && actor_1 != NULL) {
			if (actor_0->getName() == "littleBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "box") {
				//printf("小球碰方块！\n");
				removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
			}
			else if (actor_0->getName() == "bigBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "bigBall"&&actor_0->getName() == "box") {
				//printf("大球碰方块！\n");
				removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));

			}
			else if (actor_0->getName() == "littleBall"&&actor_1->getName() == "map"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "map") {
				removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
			}
			else {}
		}
	}

}
module moduleCallBack;
//删除removeActorList里面的actor
void removeActorInList() {
	int n = removeActorList.size();
	for (int i = 0; i < n; i++) {
		gScene->removeActor(*removeActorList[i]);
	}
	removeActorList.clear();
}


PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.8f, 0.8f, 0.0f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *me, 10.0f);
	//设置刚体名称
	dynamic->setName("littleBall");
	//设置碰撞的标签
	setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.5f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

PxRigidDynamic* init3rdplayer(const PxTransform& t, const PxGeometry& geometry) {
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.0f, 0.8f, 0.0f);
	PxRigidDynamic* player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
	PxVec3 position = player->getGlobalPose().p;
	cout <<"position: "<<"x: "<<position.x << " y: " << position.y << " z: " << position.z << endl;
	
	//设置刚体名称
	player->setName("3rdplayer");

	//userdata指向自己
	//dynamic->userData = dynamic;
	//设置碰撞的标签
	setupFiltering(player, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();

	player->setAngularDamping(0.5f);
	player->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//dynamic->setLinearVelocity(velocity);
	gScene->addActor(*player);
	player_ctl = player;
	return player;
}

void createBigBall() {
	//PxShape* shape = gPhysics->createShape(PxSphereGeometry(1), *gMaterial);
	PxTransform pos(PxVec3(0, 1, -18));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, pos, PxSphereGeometry(1), *gMaterial, 10.0f);
	//设置刚体名称
	body->setName("bigBall");
	//userdata指向自己
	//body->userData = body;
	//设置碰撞标签
	setupFiltering(body, FilterGroup::eBIGBALL, FilterGroup::eSTACK);
	body->setLinearVelocity(PxVec3(0, 0, 5));

	gScene->addActor(*body);
	//shape->release();
}

//创造Joint
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(1000, 100000);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	j->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);
	return j;
}
typedef PxJoint* (*JointCreateFunction)(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1);

// create a chain rooted at the origin and extending along the x-axis, all transformed by the argument t.

void createChain(const PxTransform& t, PxU32 length, const PxGeometry& g, PxReal separation, JointCreateFunction createJoint)
{
	PxVec3 offset(separation / 2, 0, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;
	cout << "localTm:" << localTm.p.x << "\t" << localTm.p.y << "\t" << localTm.p.z << "\t"
		<< localTm.q.x << "\t" << localTm.q.y << "\t" << localTm.q.z << "\t" << localTm.q.w << "\n";
	for (PxU32 i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t*localTm, g, *gMaterial, 1.0f);
		cout << "localTm:" << (t * localTm).p.x << "\t" << (t * localTm).p.y << "\t" << (t * localTm).p.z << "\t"
			<< (t * localTm).q.x << "\t" << (t * localTm).q.y << "\t" << (t * localTm).q.z << "\t" << (t * localTm).q.w << "\n";
		(*createJoint)(prev, prev ? PxTransform(offset) : t, current, PxTransform(-offset));
		gScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}
//组合Joint
void createJoint() {
	PxShape* shape1 = gPhysics->createShape(PxBoxGeometry(1, 0.4, 0.4), *gMaterial);
	PxShape* shape2 = gPhysics->createShape(PxBoxGeometry(0.4, 1, 0.4), *gMaterial);
	PxTransform pos1(PxVec3(0, 1, -10)), pos1_2(PxVec3(0, 3, -10)), pos2(PxVec3(-0.6, 2.8, -10));
	PxRigidDynamic* body1 = PxCreateDynamic(*gPhysics, pos1, *shape1, 8);
	PxRigidDynamic* body1_2 = PxCreateDynamic(*gPhysics, pos1_2, *shape2, 8);
	//PxRigidDynamic* body2 = PxCreateDynamic(*gPhysics, pos2, *shape2, 8);
	body1->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body1->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	body1_2->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body1_2->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	//body2->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//body2->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	//PxFixedJoint* j = PxFixedJointCreate(*gPhysics, body1, PxTransform(PxVec3(-0.6,0.6,0)), body2, PxTransform(PxVec3(0,-1.2,0)));
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, body1, PxTransform(PxVec3(1, 0, 0)), body1_2, PxTransform(PxVec3(0, -1, 0)));
	j->setBreakForce(1000000, 10);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	j->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);

	gScene->addActor(*body1);
	gScene->addActor(*body1_2);
	//gScene->addActor(*body2);
	shape1->release();
	shape2->release();

}
//飞机刚体
void createAirPlane() {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);
	//PxTransform initPos(PxVec3(2, 1, -15), PxQuat(PxPi / 6, PxVec3(0, 0, 1)));
	PxTransform initPos(PxVec3(2, 1, -15));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, initPos, *shape, 8);

	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	airPlane = body;
	gScene->addActor(*body);

	//尾翼
	PxShape* shape2 = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);

	shape->release();
}
//改变飞机姿态与速度，使其做圆周运动
void changeAirPlaneVelocity() {
	angelAirPlane += 1;
	angelAirPlane = angelAirPlane % 360;
	PxQuat rot(PxPi / 180 * angelAirPlane, PxVec3(0, 0, 1));
	headForward = rot.rotate(PxVec3(1, 0, 0));
	airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot));
	airPlane->setLinearVelocity(5 * headForward);
}

PxRigidDynamic* player;
PxRigidDynamic* initPlayer() {

	PxTransform pos(PxVec3(4, 1, 13));
	player = PxCreateDynamic(*gPhysics, pos, PxSphereGeometry(0.5), *gMaterial, 10.0f);
	//设置刚体名称
	player->setName("player");
	//设置碰撞标签
	//player->setLinearVelocity(PxVec3(0, 0, -5));
	gScene->addActor(*player);
	return player;
}


void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent) {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent, PxQuat(45, PxVec3(1, 0, 0)));
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			//设置刚体名称
			body->setName("box");
			//userdata指向自己
			//body->userData = body;

			//设置碰撞的标签
			setupFiltering(body, FilterGroup::eSTACK, FilterGroup::eBALL | FilterGroup::eBIGBALL);
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

void createModel(std::string path, int scale, PxVec3& offset) {

}

module moduleCallBack;
PxReal					stackZ = 3.0f;
extern Camera camera;
extern Shader* envShader;
clock_t	 lockFrame_last = 0, lockFrame_current = 0;


void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	PxInitExtensions(*gPhysics, gPvd);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD; // 开启CCD：continuous collision detection
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = testCCDFilterShader;
	//sceneDesc.filterShader = testCollisionFilterShader;
	//注册onContact
	sceneDesc.simulationEventCallback = &moduleCallBack;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	gScene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 3; i++)
		//createStack(PxTransform(PxVec3(0, 2, stackZ -= 3.0f)), 10, 0.1f);
	//createBigBall();
	
	//生成第三人称角色
	//PxTransform born_pos(PxVec3(0, 1, -7));
	//init3rdplayer(born_pos, PxSphereGeometry(1.0f));
	//createBigBall();

	createAirPlane();
	//createChain(PxTransform(PxVec3(0.0f, 2.0f, -10.0f)), 5, PxBoxGeometry(1.0f, 0.3f, 0.3f), 2.0f, createBreakableFixed);
	//createJoint();

	camera.setTarget(player);

	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Stadium/sports stadium.obj", envShader, false);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/cityislands/City Islands/City Islands.obj", envShader);


	if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}



void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	//锁帧
	lockFrame_current = clock();//当前时钟
	if ((lockFrame_current - lockFrame_last) < 16) {
		//skip，1000clocks/s，则一帧约16ms（60帧）
		Sleep(16 - (lockFrame_current - lockFrame_last));
	}
	else {
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
		removeActorInList();
		changeAirPlaneVelocity();
		lockFrame_last = lockFrame_current;//每执行一帧，记录上一帧（即当前帧）时钟
	}

}

void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->release();
	gDispatcher->release();
	gPhysics->release();
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();

	gFoundation->release();

	printf("program exit.\n");
}



extern int myRenderLoop();
int snippetMyMain(int, const char*const*)
{
#ifdef RENDER_SNIPPET

	return myRenderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for (PxU32 i = 0; i < frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}
