#include <ctype.h>
#include <iostream>
#include <ctime>

#include "Creater.h"

#define PI 3.1415926

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
module moduleCallBack;




void createModel(std::string path, int scale, PxVec3& offset) {

}

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
	
	for (PxU32 i = 0; i < 3; i++)
		createStack(PxTransform(PxVec3(0, 2, stackZ -= 3.0f)), 10, 0.1f);
	//createBigBall();

	//生成第三人称角色
	PxTransform born_pos(PxVec3(0, 1, -7));
	//init3rdplayer(born_pos, PxSphereGeometry(0.5f));
	initvehicle(born_pos, PxSphereGeometry(0.5f));
	//createBigBall();

	//createAirPlane();

	//camera.setTarget(player);
	camera.setTarget(vehicle);

	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Stadium/sports stadium.obj", envShader, false);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/cityislands/City Islands/City Islands.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/chevrolet/Chevrolet_Camaro_SS_Low.obj", envShader,false);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/suv/Models/1.obj", envShader, false);
	//createModel(glm::vec3(10.0f,50.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/vehicle/airplane/11803_Airplane_v1_l1.obj", envShader,false);
	//model\vehicle\suv\Models Transport Shuttle_obj.obj
	//createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", envShader, false);
		


	//ball = new Ball(glm::vec3(0.0f, 0.20f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/football/soccer ball.obj", envShader);

	if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}



bool createSpecialStaticModel(BaseModel* model, bool preLoad, bool ifStatic) {
	ObjLoader loader(model, MESH_TYPE::TRIANGLE);
	if (ifStatic)
		loader.createStaticActorAndAddToScene(); //静态刚体
	else
		loader.createDynamicActorAndAddToScene();
	return true;
}


void stepPhysics(bool interactive)
{
	PX_UNUSED(interactive);
	//锁帧
	/*lockFrame_current = clock();//当前时钟
	if ((lockFrame_current - lockFrame_last) < 16) {
		//skip，1000clocks/s，则一帧约16ms（60帧）
		Sleep(16 - (lockFrame_current - lockFrame_last));
	}
	else {
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
		removeActorInList();
		//changeAirPlaneVelocity();
		lockFrame_last = lockFrame_current;//每执行一帧，记录上一帧（即当前帧）时钟
	}*/
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
