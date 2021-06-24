#include <ctype.h>
#include <iostream>
#include <ctime>

#include "Creater.h"
#include "Characters.h"
#define PI 3.1415926

using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
module moduleCallBack;
AirPlane				*Plane_1;
//第三人称角色位置
PxTransform born_pos(PxVec3(10, 0, -7));

void createModel(std::string path, int scale, PxVec3& offset) {

}

PxReal					stackZ = 3.0f;
extern Camera camera;
extern Shader* envShader;
clock_t					lockFrame_last = 0, lockFrame_current = 0;

guntower GunTower;

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	PxInitExtensions(*gPhysics, gPvd);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));

	PxRegisterParticles(*gPhysics);

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
	//createAbleBreakWall();


	init3rdplayer(born_pos, PxSphereGeometry(0.5f));
	//initvehicle(born_pos, PxSphereGeometry(0.5f));
	//createBigBall();
   

	/*glm::vec3 pos1(5.0f, 5.0f, 0.0f);
	GunTower.initguntower(pos1);*/
	vector<glm::vec3>pos_list;
	glm::vec3 pos1(5.0f, 5.0f, 0.0f);
	int nb_tower = 5;
	for (int i = 0; i < nb_tower; i++) {
		pos_list.push_back(pos1);
		pos1.x += i * 1.0f;
		pos1.y += i * 1.0f;
	}
	GunTower.initlist(pos_list);
	

	//createAirPlane();
	PxRigidDynamic* temp = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(0.0f, 20.0f, -10.0f), glm::vec3(0.1f, 0.1f, 0.1f), 
		"model/vehicle/Fighter-jet/fighter_jet.obj", envShader, false));
	Plane_1 = new AirPlane(PxVec3(0,0,1),PxVec3(0,1,0),PxVec3(-1,0,0), temp);

	//camera.setTarget(player);
	camera.setTarget(Plane_1->body);
	//camera.setTarget(vehicle);

	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Stadium/sports stadium.obj", envShader, false);
	createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/cityislands/City Islands/City Islands.obj", envShader);
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
	lockFrame_current = clock();//当前时钟
	/*if ((lockFrame_current - lockFrame_last) < 16) {
		//skip，1000clocks/s，则一帧约16ms（60帧）
		Sleep(16 - (lockFrame_current - lockFrame_last));
	}
	else {
		gScene->simulate(((lockFrame_current - lockFrame_last) / 16.0f) / 60.0f);
		gScene->fetchResults(true);
		removeActorInList();
		changeAirPlaneVelocity();
		GunTower.runguntower(player);
		lockFrame_last = lockFrame_current;//每执行一帧，记录上一帧（即当前帧）时钟
	
	}*/
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
	Plane_1->manualControlAirPlane();

	//GunTower.runguntower(player);

	removeActorInList();
	//gScene->simulate(((lockFrame_current - lockFrame_last) / 16.f) / 60.f);
	/*gScene->fetchResults(true);
	removeActorInList();
	changeAirPlaneVelocity();
	GunTower.runguntower(player);
	lockFrame_last = lockFrame_current;//每执行一帧，记录上一帧（即当前帧）时钟
	*/
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
