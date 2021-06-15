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
#define PI 3.1415926

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
PxSimulationEventCallback *myCallBack;
module moduleCallBack;

PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics = NULL;

PxCooking*				gCooking = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene = NULL;

PxMaterial*				gMaterial = NULL;

PxPvd*                  gPvd = NULL;

PxReal stackZ = 10.0f;

PxRigidDynamic*			body_1 = NULL;

PxRigidStatic* bodyToRemove = NULL;


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


PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.8f, 0.8f, 0.0f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *me, 10.0f);
	me->release();
	//设置trigger的参数
	body_1 = dynamic;
	printf("createDynamic!\n");
	/*PxShape* treasureShape;
	body_1->getShapes(&treasureShape, 1);
	treasureShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	treasureShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);*/
	dynamic->setAngularDamping(0.5f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
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
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}
void createWall() {
	PxTransform original(PxVec3(PxReal(0), PxReal(0), PxReal(0)));

	PxShape* shape1 = gPhysics->createShape(PxBoxGeometry(3, 1, 0.1), *gMaterial);
	PxShape* shape2 = gPhysics->createShape(PxBoxGeometry(0.1, 1, 3), *gMaterial);

	PxTransform local1(PxVec3(PxReal(3), PxReal(1), PxReal(3)));
	PxTransform local2(PxVec3(PxReal(3), PxReal(1), PxReal(4)));
	PxTransform local3(PxVec3(PxReal(3), PxReal(1), PxReal(5)));
	PxTransform local4(PxVec3(PxReal(6), PxReal(1), PxReal(6)), PxQuat(0, 1, 0, 0));

	PxRigidStatic* body1 = gPhysics->createRigidStatic(original.transform(local1));	std::cout << "body1:" << body1 << "\n";
	body1->attachShape(*shape1);
	gScene->addActor(*body1);
	PxRigidStatic* body2 = gPhysics->createRigidStatic(original.transform(local2)); std::cout << "body2:" << body2 << "\n";
	body2->attachShape(*shape2);
	gScene->addActor(*body2);
	PxRigidStatic* body3 = gPhysics->createRigidStatic(original.transform(local3)); std::cout << "body3:" << body3 << "\n";
	body3->attachShape(*shape1);
	gScene->addActor(*body3);
	PxRigidStatic* body4 = gPhysics->createRigidStatic(original.transform(local4));
	body4->attachShape(*shape1);
	gScene->addActor(*body4);/**/
	bodyToRemove = body4;

	shape1->release();
	shape2->release();
}


void remove() {
	gScene->removeActor(*bodyToRemove);
}

void createBowl() {
	PxTransform original(PxVec3(PxReal(150), PxReal(0), PxReal(0)));

	PxShape* bowlShape = gPhysics->createShape(PxCapsuleGeometry(2, 3.5), *gMaterial);
	PxTransform local1(PxVec3(PxReal(-5.5), PxReal(2), PxReal(0)));
	PxTransform local2(PxVec3(PxReal(5.5), PxReal(4), PxReal(0)));
	PxTransform local3(PxVec3(PxReal(16.5), PxReal(5.5), PxReal(0)));
	PxTransform local4(PxVec3(PxReal(27.5), PxReal(8), PxReal(0)));

	PxRigidStatic* body1 = gPhysics->createRigidStatic(original.transform(local1));
	body1->attachShape(*bowlShape);
	/*body1->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	body1->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,true);
	body1->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);*/
	gScene->addActor(*body1);
	PxRigidStatic* body2 = gPhysics->createRigidStatic(original.transform(local2));
	body2->attachShape(*bowlShape);
	gScene->addActor(*body2);
	PxRigidStatic* body3 = gPhysics->createRigidStatic(original.transform(local3));
	body3->attachShape(*bowlShape);
	gScene->addActor(*body3);
	PxRigidStatic* body4 = gPhysics->createRigidStatic(original.transform(local4));
	body4->attachShape(*bowlShape);
	gScene->addActor(*body4);

	bowlShape->release();
}

void createModel(std::string path, int scale, PxVec3& offset) {

}

PxRigidActor* body_0 = NULL;
void testTrigger() {
	PxTransform original(PxVec3(PxReal(0), PxReal(0), PxReal(0)));//坐标原点
	PxShape* shape1 = gPhysics->createShape(PxBoxGeometry(100, 30, 3), *gMaterial);//长宽高为200 60 6的墙
	PxTransform local4(PxVec3(PxReal(-200), PxReal(30), PxReal(0)), PxQuat(0, 1, 0, 0));
	PxRigidStatic* body4 = gPhysics->createRigidStatic(original.transform(local4)); std::cout << "body4:" << body4 << "\n";


	module wall(original, shape1, local4, body4);
	wall.body0->attachShape(*shape1); std::cout << "wall.body0:" << wall.body0 << "\n";
	/*PxShape* treasureShape;
	wall.body0->getShapes(&treasureShape, 1);
	treasureShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	treasureShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);*/
	body_0 = wall.body0;
	gScene->addActor(*wall.body0);

	/*body4->attachShape(*shape1);

	gScene->addActor(*body4);*/
	bodyToRemove = body4;

	shape1->release();
}
bool toRemove = false;
void module::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	printf("Enter onTrigger!\n");
	for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) {
			printf("Enter first 'if' !\n");
			continue;
		}

		if ((pairs[i].otherActor == body_0) && (pairs[i].triggerActor == body_1))
		{
			toRemove = true;
			printf("module::onTrigger!\n");
		}
	}
}
void module::onWake(PxActor** actor, PxU32 count) {
	std::cout << "Enter onWake!\n";
	if (toRemove) remove();
	return;
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
	//注册onTrigger
	myCallBack = new module();
	sceneDesc.simulationEventCallback = myCallBack;
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

	for (PxU32 i = 0; i < 5; i++)
		createStack(PxTransform(PxVec3(0, 2, stackZ -= 10.0f)), 10, 0.1f);

	//createWall();
	//createBowl();
	//testTrigger();

	//std::string path = "model/street/Street environment_V01.obj";
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
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

void keyPress(unsigned char key, const PxTransform& camera)
{
	switch (toupper(key))
	{
	case 'B':	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);						break;
	case 'F':	createDynamic(camera, PxSphereGeometry(0.1f), camera.rotate(PxVec3(0, 0, -1)) * 20);	break;
	case 'R':	remove();	break;
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
