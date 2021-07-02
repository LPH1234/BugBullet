﻿#include <ctype.h>
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
Media MediaPlayer;

void createModel(std::string path, int scale, PxVec3& offset) {}

PxReal					stackZ = 3.0f;
extern Camera camera;
extern Shader* envShader;
clock_t					lockFrame_last = 0, lockFrame_current = 0;
vector<Player*>		tankList(4,nullptr);
//Player* vehicle;
guntower GunTower;
bonus Bonus;

vector<glm::vec3>east_island_pos_list = { glm::vec3(247.0f, 7.6f, 29.3f),glm::vec3(248.f, 5.5f, 80.0f),glm::vec3(248.0f, 5.5f, -141.0f),
										glm::vec3(361.0f, 7.6f, -138.0f),glm::vec3(361.0f, 7.6f, -55.0f),glm::vec3(313.0f, 7.6f, 29.0f),
										glm::vec3(356.0f, 7.6f, -243.0f),glm::vec3(380.0f, 7.6f, -136.0f),
										glm::vec3(136.5f, 5.4f, -140.0f),glm::vec3(136.5f, 5.4f, 22.0f),glm::vec3(136.5f, 5.4f, 60.0f),
										glm::vec3(136.5f, 5.4f, -23.0f),glm::vec3(136.5f, 5.4f, -45.0f) };
	
vector<glm::vec3>south_island_pos_list = { glm::vec3(-1.6f, 5.6f, 23.4f),glm::vec3(-6.7f, 5.6f, 31.8f),glm::vec3(-88.6f, 5.6f, 18.3f),
										glm::vec3(-85.3f, 5.6f, -30.5f),glm::vec3(-85.3f, 5.6f, 107.4f),glm::vec3(88.9f, 5.6f, -49.9f),
										 glm::vec3(131.0f, 5.6f, 22.3f),glm::vec3(130.0f, 5.6f, 60.3f),glm::vec3(130.3f, 5.6f, -140.0f),
										glm::vec3(24.8f, 5.6f, -103.9f),glm::vec3(23.8f, 5.6f, -12.5f) };
vector<glm::vec3>north_island_pos_list = { glm::vec3(24.8f,7.6f,-261.8f),glm::vec3(24.6f,7.6f,-330.8f),glm::vec3(25.1f,7.6f,-394.6f),
										glm::vec3(26.8f,7.6f,-464.3f),glm::vec3(-52.8f,7.6f,-484.3f),glm::vec3(-87.1f,7.6f,-384.8f),
										glm::vec3(-19.7f,7.6f,-256.8f) };
//初始化坦克，一共四辆
void initTank() {
	int index[4][2] = { {8,3},{5,7},{1,6},{12,13} };
	PxVec3 dir[4] = { PxVec3(-1,0,0),PxVec3(0,0,-1),PxVec3(1,0,0),PxVec3(0,0,-1) };
	for (int i = 0; i < 4; i++) {
		PxVec3 start; glmVec3ToPxVec3 (east_island_pos_list[index[i][0] - 1],start);
		PxVec3 end; glmVec3ToPxVec3(east_island_pos_list[index[i][1] - 1], end);
		PxRigidDynamic* input_tank = reinterpret_cast<PxRigidDynamic*>(createModel(east_island_pos_list[index[i][0]-1], glm::vec3(1.0f, 1.0f, 1.0f),
			"model/vehicle/ls2fh1gay9-PGZ-95 AA/PGZ-99.obj", envShader, false));
		input_tank->setMass(10.f);
		//input_tank->setMassSpaceInertiaTensor(PxVec3(0.f));

		tankList[i] = new Player(input_tank, Plane_1, start, end, dir[i]);

		
	}
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(1, 1, 1), *gMaterial);
	PxRigidDynamic* box1 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(300.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box2 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(305.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box3 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(400.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box4 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(405.0f, 13.f, -140.0f)), *shape, 100);
	PxRigidDynamic* box5 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(410.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box6 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(415.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box7 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(420.0f, 7.6f, -140.0f)), *shape, 100);
	PxRigidDynamic* box8 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(425.0f, 13.f, -140.0f)), *shape, 100);
	box1->attachShape(*shape);
	box2->attachShape(*shape);
	box3->attachShape(*shape);
	box4->attachShape(*shape);
	box5->attachShape(*shape);
	box6->attachShape(*shape);
	box7->attachShape(*shape);
	box8->attachShape(*shape);
	gScene->addActor(*box1);
	gScene->addActor(*box2);
	gScene->addActor(*box3);
	gScene->addActor(*box4);
	gScene->addActor(*box5);
	gScene->addActor(*box6);
	gScene->addActor(*box7);
	gScene->addActor(*box8);
}
void tankAutoMove() {
	for (int i = 0; i < 4; i++) {
		tankList[i]->automove();
	}
}
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
	//sceneDesc.filterShader = testCCDFilterShader;
	sceneDesc.filterShader = testCCDFilterShader2;
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
	groundPlane->userData = new UserData(1, "border", DATATYPE::TRIGGER_TYPE::BORDER);
	PxShape* treasureShape;
	groundPlane->getShapes(&treasureShape, 1);
	treasureShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	treasureShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	gScene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 3; i++)
		//createStack(PxTransform(PxVec3(0, 2, stackZ -= 3.0f)), 10, 0.1f);
	//createBigBall();
	//createAbleBreakWall();
	//createBreakableWall();


	//init3rdplayer(born_pos, PxSphereGeometry(0.5f));
	//initvehicle(born_pos, PxSphereGeometry(0.5f));
	//createBigBall();


	/*glm::vec3 pos1(5.0f, 5.0f, 0.0f);
	GunTower.initguntower(pos1);*/
	vector<glm::vec3>pos_list;

	glm::vec3 pos1(5.0f, 5.0f, 0.0f); 
	int nb_tower = 5;
	/*for (int i = 0; i < nb_tower; i++) {
		pos_list.push_back(pos1);
		pos1.x += i * 1.0f;
		pos1.y += i * 1.0f;
	}*/
	/*for (int i = 0; i < east_island_pos_list.size(); i++) {
		pos_list.push_back(east_island_pos_list[i]);
	}*/
	/*for (int i = 0; i < south_island_pos_list.size(); i++) {
		pos_list.push_back(south_island_pos_list[i]);
	}*/
	//加载炮塔
	for (int i = 0; i < north_island_pos_list.size(); i++) {
		pos_list.push_back(north_island_pos_list[i]);
	}
	GunTower.initlist(pos_list);
	vector<glm::vec3> supply_pos_list = { east_island_pos_list[1],south_island_pos_list[2] };
	Bonus.initlist(supply_pos_list);

	//加载飞机
	PxRigidDynamic* temp = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(136.0f, 20.0f, -10.0f), glm::vec3(0.3f, 0.3f, 0.3f),
		"model/vehicle/Fighter-jet/fighter_jet.obj", envShader, false));
	Plane_1 = new AirPlane(PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0), temp);

	//加载坦克
	initTank();

	/*PxRigidDynamic* input_tank = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(131.f, 7.0f, 22.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		"model/vehicle/ls2fh1gay9-PGZ-95 AA/PGZ-99.obj", envShader, false));*/
	//setupFiltering(input_tank, FilterGroup::eTANK, FilterGroup::eMISILE);
	//testFilter();
	testTriggerWall();

	//testTriggerCollection();
	//vehicle = new Player(input_tank, Plane_1);

	{
		//PxRigidStatic* staticTank = reinterpret_cast<PxRigidStatic*>(createModel(glm::vec3(-10.f, 7.0f, 22.0f), glm::vec3(0.75f, 0.75f, 0.75f),
		//	"model/vehicle/ls2fh1gay9-PGZ-95 AA/PGZ-99.obj", envShader));
		//setupFiltering(staticTank, FilterGroup::eMAP, FilterGroup::eMISILE);

		///*PxRigidStatic* staticBox = reinterpret_cast<PxRigidStatic*>(createModel(glm::vec3(0.f, 7.0f, 22.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		//	"model/test/cube.obj", envShader));
		//setupFiltering(staticBox, FilterGroup::eMAP, FilterGroup::eMISILE);*/

		//PxRigidActor* dynamicTank = createModel(glm::vec3(10.f, 7.0f, 22.0f), glm::vec3(0.75f, 0.75f, 0.75f),
		//	"model/vehicle/ls2fh1gay9-PGZ-95 AA/PGZ-99.obj", envShader,false);
		//setupFiltering(dynamicTank, FilterGroup::eMAP, FilterGroup::eMISILE);
		//
		///*PxRigidDynamic* dynamicBox = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(20.f, 7.0f, 22.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		//	"model/test/cube.obj", envShader,false));
		//setupFiltering(dynamicBox, FilterGroup::eMAP, FilterGroup::eMISILE);*/
	}

	//camera.setTarget(player);
	//camera.setTarget(Plane_1);
	camera.setTarget(tankList[0]);

	PxRigidActor* Map = nullptr;
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f),"model/street/Street environment_V01.obj", envShader);
	//Map = createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/cityislands/City Islands/City Islands.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/street/Street environment_V01.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/env/Castelia-City/Castelia City.obj", envShader);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/env/Stadium/sports stadium.obj", envShader, false);
	Map = reinterpret_cast<PxRigidStatic*>(createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 
		"model/env/cityislands/City Islands/City Islands.obj", envShader));
	//setupFiltering(Map, FilterGroup::eMAP, FilterGroup::eMISILE);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/chevrolet/Chevrolet_Camaro_SS_Low.obj", envShader,false);
	//createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/suv/Models/1.obj", envShader, false);
	//createModel(glm::vec3(10.0f,50.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), "model/vehicle/airplane/11803_Airplane_v1_l1.obj", envShader,false);
	//model\vehicle\suv\Models Transport Shuttle_obj.obj
	//createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", envShader, false);
	Map->setName("map");

	Map->userData = new UserData(1, "map", DATATYPE::ACTOR_TYPE::MAP);

	//ball = new Ball(glm::vec3(0.0f, 0.20f, 0.0f), glm::vec3(0.0025f, 0.0025f, 0.0025f), "model/football/soccer ball.obj", envShader);

	if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}
//更新炮塔血条
//void updateGuntowerInList() {
//	int count = GunTower.count;
//	for (int i = 0; i < count; i++) {
//		int currentHealth = GunTower.health_list[i];
//		if (currentHealth == 0) {
//			if (GunTower.blood_body_list[i]) {
//				gScene->removeActor(*GunTower.blood_body_list[i]);
//			}
//			GunTower.blood_body_list[i] = nullptr;
//		}
//		else {
//			float l = currentHealth / 50.0*3.0;
//			const PxU32 numShapes = GunTower.blood_body_list[i]->getNbShapes();
//			PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*)*numShapes);
//			GunTower.blood_body_list[i]->getShapes(shapes, numShapes);
//			for (PxU32 j = 0; j < numShapes; j++)
//			{
//				PxShape* shape = shapes[j];
//				shape->setGeometry(PxBoxGeometry(l, 0.1f, 0.1f));
//			}
//			free(shapes);
//		}
//	}
//}
void beforeStepPhysics() {
	addForceToPartivleSystem(physicsParticleSystemList);
}

void stepPhysics(bool interactive)
{
	if (!interactive)
		return;

	beforeStepPhysics();

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
	//vehicle->automove();
	tankAutoMove();
	Plane_1->manualControlAirPlane4();
	//Plane_1->formcloud();
	//Plane_1->formmisslecloud();
	GunTower.runguntower(Plane_1->body);
	Bonus.runsupply();
	//GunTower.runguntower(vehicle->getRigid());
	addBonusInList();
	
	removeActorInList();
	updateTankInList();
	updateGuntowerInList();
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
