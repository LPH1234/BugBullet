#include <ctype.h>
#include <iostream>
#include "PxPhysicsAPI.h"
#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include "../Render/objLoader.h"
#include "../Utils/module.h"
#include "../Render/models.h"
#include "../Render/Render.h"
#include<time.h>
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

PxReal stackZ = 10.0f;

vector<PxActor*> removeActorList;
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
bool createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool preLoad = false, bool ifStatic = true);
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
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags ^= PxPairFlag::eNOTIFY_TOUCH_FOUND;

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
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxRigidActor* actor_0 = (PxRigidActor*)(pairHeader.actors[0]);
		PxRigidActor* actor_1 = (PxRigidActor*)(pairHeader.actors[1]);
		if (actor_0 != NULL && actor_1 != NULL) {
			if (actor_0->getName() == "littleBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "box") {
				printf("小球碰方块！\n");
				removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
			}
			else if (actor_0->getName() == "bigBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "bigBall"&&actor_0->getName() == "box") {
				printf("大球碰方块！\n");
				removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
			}
			else {}
		}
		/*const PxContactPair& cp = pairs[i];
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			printf("碰撞\n");
		}*/
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
	//userdata指向自己
	//dynamic->userData = dynamic;
	//设置碰撞的标签
	setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.5f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

PxRigidDynamic* init3rdplyer(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0)) {
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.8f, 0.8f, 0.0f);
	PxRigidDynamic* player = PxCreateDynamic(*gPhysics, t, geometry, *me, 10.0f);
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
	return player;
}

void createBigBall() {
	//PxShape* shape = gPhysics->createShape(PxSphereGeometry(1), *gMaterial);
	PxTransform pos(PxVec3(0, 1, 13));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, pos, PxSphereGeometry(1), *gMaterial, 10.0f);
	//设置刚体名称
	body->setName("bigBall");
	//userdata指向自己
	//body->userData = body;
	//设置碰撞标签
	setupFiltering(body, FilterGroup::eBIGBALL, FilterGroup::eSTACK);
	body->setLinearVelocity(PxVec3(0,0,-5));
	gScene->addActor(*body);
	//shape->release();
}

void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			//设置刚体名称
			body->setName("box");
			//userdata指向自己
			//body->userData = body;
			//设置碰撞的标签
			setupFiltering(body, FilterGroup::eSTACK, FilterGroup::eBALL | FilterGroup::eBIGBALL);
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

void createModel(std::string path, int scale, PxVec3& offset) {

}

extern Shader* envShader;
PlainModel *street;
Ball *ball;

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

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

	for (PxU32 i = 0; i < 3; i++)
		createStack(PxTransform(PxVec3(0, 2, stackZ -= 10.0f)), 10, 0.1f);
	createBigBall();


	//std::string path = "model/street/Street environment_V01.obj";
	//createStaticModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	//createStaticModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createStaticModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/cityislands/City Islands/City Islands.obj", envShader);



	//ball = new Ball(glm::vec3(0.0f, 0.20f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/football/soccer ball.obj", envShader);

	if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}

bool createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool preLoad, bool ifStatic) {
	if (FileUtils::isFileExist(modelPath)) {
		BaseModel* model = new PlainModel(pos, scale, modelPath, shader);
		ObjLoader loader(model, preLoad);
		if (ifStatic)
			loader.createStaticActorAndAddToScene(); // 静态刚体
		else
			loader.createDynamicActorAndAddToScene(); // 动态刚体
		Logger::debug("创建完成");
	}
	else {
		Logger::error("文件不存在：" + modelPath);
		return false;
	}
	return true;
}
bool createSpecialStaticModel(BaseModel* model, bool preLoad, bool ifStatic) {
	ObjLoader loader(model, preLoad);
	if (ifStatic)
		loader.createStaticActorAndAddToScene(); //静态刚体
	else
		loader.createDynamicActorAndAddToScene();
	return true;
}

void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
	removeActorInList();
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
bool autoshooting = true;
clock_t last = 0;
void keyPress(unsigned char key, const PxTransform& camera)
{
	switch (toupper(key))
	{
	case 'B':	
		createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);					
		break;
	case 'F':
		if (autoshooting) {
			createDynamic(camera, PxSphereGeometry(0.1f), camera.rotate(PxVec3(0, 0, -1)) * 20);
			break;
		}
		else {
			clock_t now = clock();
			if (now - last > 1000) {
				createDynamic(camera, PxSphereGeometry(0.1f), camera.rotate(PxVec3(0, 0, -1)) * 20);
				last = now;
			}
			break;
		}
	case 'T':
		autoshooting = !autoshooting;
		if (autoshooting) {
			cout << "切换成全自动" << endl;
		}
		else {
			cout << "切换成半自动" << endl;
		}
		break;
	}
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
