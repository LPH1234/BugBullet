#include <ctype.h>
#include <iostream>
#include <ctime>
#include "Creater.h"
#include "Characters.h"
#define PI 3.1415926

using namespace physx;

PxDefaultAllocator						gAllocator;
PxDefaultErrorCallback					gErrorCallback;
module									moduleCallBack;				//回调函数
PxReal									stackZ = 3.0f;
clock_t									lockFrame_last = 0;			//锁帧
clock_t									lockFrame_current = 0;
AirPlane								*Plane_1 = nullptr;			//玩家飞机
AirPlane_AI								*Plane_AI = nullptr;		//用于测试单个AI飞机
MissileManager							*ManageMissile = nullptr;	//导弹管理器
Media									MediaPlayer;				//声音播放器
vector<Player*>							tankList(4, nullptr);		//坦克链表
vector<AirPlane_AI*>					AI_PlaneList(4, nullptr);	//AI飞机链表
vector<AirPlane_AI*>					tempList(1, nullptr);		//测试
//Player*								  vehicle;
guntower								GunTower;
bonus									Bonus;
PxRigidActor*							Map = nullptr;				//地图
bool									Level[3]{ false,false,false };
set<PxRigidActor*>						exsitBonusList;				//当前存在的道具

void createModel(std::string path, int scale, PxVec3& offset) {}

extern Camera camera;
extern Shader* envShader;

vector<glm::vec3>east_island_pos_list = { glm::vec3(247.0f, 7.6f, 29.3f),glm::vec3(248.f, 5.5f, 80.0f),glm::vec3(248.0f, 5.5f, -141.0f),
										glm::vec3(361.0f, 7.6f, -138.0f),glm::vec3(361.0f, 7.6f, -55.0f),glm::vec3(313.0f, 7.6f, 29.0f),
										glm::vec3(356.0f, 7.6f, -243.0f),glm::vec3(350.0f, 7.6f, -136.0f),
										glm::vec3(136.5f, 5.4f, -140.0f),glm::vec3(136.5f, 5.4f, 22.0f),glm::vec3(136.5f, 5.4f, 60.0f),
										glm::vec3(136.5f, 5.4f, -23.0f),glm::vec3(136.5f, 5.4f, -45.0f) };

vector<glm::vec3>south_island_pos_list = { glm::vec3(-1.6f, 5.6f, 23.4f),glm::vec3(-6.7f, 5.6f, 31.8f),glm::vec3(-88.6f, 5.6f, 18.3f),
										glm::vec3(-85.3f, 5.6f, -30.5f),glm::vec3(-85.3f, 5.6f, 107.4f),glm::vec3(88.9f, 5.6f, -49.9f),
										 glm::vec3(131.0f, 5.6f, 22.3f),glm::vec3(130.0f, 5.6f, 60.3f),glm::vec3(130.3f, 5.6f, -140.0f),
										glm::vec3(24.8f, 5.6f, -103.9f),glm::vec3(23.8f, 5.6f, -12.5f) };

vector<glm::vec3>north_island_pos_list = { glm::vec3(24.8f,7.6f,-261.8f),glm::vec3(24.6f,7.6f,-330.8f),glm::vec3(25.1f,7.6f,-394.6f),
										glm::vec3(26.8f,7.6f,-464.3f),glm::vec3(-52.8f,7.6f,-484.3f),glm::vec3(-87.1f,7.6f,-384.8f),
										glm::vec3(-19.7f,7.6f,-256.8f) };

//初始化地图
void initMap() {
	Map = reinterpret_cast<PxRigidStatic*>(createModel(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		"model/env/cityislands/City Islands/City Islands.obj", envShader));
	Map->setName("map");
	Map->userData = new UserData(1, "map", DATATYPE::ACTOR_TYPE::MAP);
}

//初始化玩家飞机
void initMyAirPlane() {
	PxRigidDynamic* temp = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(0.0f, 10.0f, -10.0f), glm::vec3(0.3f, 0.3f, 0.3f),
		"model/vehicle/Fighter-jet/fighter_jet.obj", envShader, false));
	Plane_1 = new AirPlane(PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0), temp, ManageMissile, AI_PlaneList);
}

//初始化炮塔
void initGunTower() {
	vector<glm::vec3>pos_list;
	//加载炮塔
	for (int i = 0; i < north_island_pos_list.size(); i++) {
		pos_list.push_back(north_island_pos_list[i]);
	}
	GunTower.initlist(pos_list);
}

//初始化道具等
void initBonus() {
	vector<glm::vec3> supply_pos_list = { east_island_pos_list[1],south_island_pos_list[2] };
	Bonus.initlist(supply_pos_list);
}

//初始化坦克，一共2辆
void initTank() {
	int index[4][2] = { {1,6},{12,13},{8,3},{5,7} };
	PxVec3 dir[4] = { PxVec3(1,0,0),PxVec3(0,0,-1),PxVec3(-1,0,0),PxVec3(0,0,-1) };
	for (int i = 0; i < 2; i++) {
		PxVec3 start; glmVec3ToPxVec3(east_island_pos_list[index[i][0] - 1], start);
		PxVec3 end; glmVec3ToPxVec3(east_island_pos_list[index[i][1] - 1], end);
		PxRigidDynamic* input_tank = reinterpret_cast<PxRigidDynamic*>(createModel(east_island_pos_list[index[i][0] - 1], glm::vec3(1.0f, 1.0f, 1.0f),
			"model/vehicle/ls2fh1gay9-PGZ-95 AA/PGZ-99.obj", envShader, false));
		//input_tank->setMass(100.f);

		tankList[i] = new Player(input_tank, Plane_1, start, end, dir[i]);


	}
}
//坦克移动
void tankAutoMove() {
	for (int i = 0; i < 2; i++) {
		tankList[i]->automove();
	}
}

//初始化AI飞机，4架
vector<AirPlane_AI*> initAI_Plane() {
	glm::vec3 posList[4] = { glm::vec3(0.f,80.f,-10.f),glm::vec3(80.f,80.f,20.f),glm::vec3(-80.f,100.f,-80.f),glm::vec3(0.f,100.f,30.f) };
	for (int i = 0; i < 4; i++) {
		cout << "initAI_Plane!\n";
		//if (AI_PlaneList[i]==nullptr) {
		PxRigidDynamic* plane_AI = reinterpret_cast<PxRigidDynamic*>(createModel(posList[i], glm::vec3(0.3f, 0.3f, 0.3f),
			"model/vehicle/Fighter-jet/fighter_jet.obj", envShader, false));
		AI_PlaneList[i] = new AirPlane_AI(PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0), plane_AI, ManageMissile, Plane_1);
		UserData* tempData = reinterpret_cast<UserData*>(AI_PlaneList[i]->body->userData);
		tempData->id = i;
		//cout << tempData->id << "AI初始化完成！\n";
	//}
	}
	return AI_PlaneList;
}
//AI飞机飞行
void AI_PlaneAutoFly() {
	for (int i = 0; i < 4; i++) {
		if (AI_PlaneList[i]->alive)AI_PlaneList[i]->autoFlying();
	}
}

bool isToChangeLevel() {
	bool isSuccessful = false;
	if (Level[0]) {
		for (int i = 0; i < 2; i++) {
			if (tankList[i]->alive)
				return false;
		}
		Level[0] = false;
		Level[1] = true;
		return true;
	}
	else if (Level[1]) {
		int count = GunTower.tower_list.size();
		for (int i = 0; i < count; i++) {
			if (GunTower.health_list[i] != 0)
				return false;
		}
		Level[1] = false;
		Level[2] = true;
		return true;
	}
	else if (Level[2]) {
		int count = AI_PlaneList.size();
		for (int i = 0; i < count; i++) {
			if (AI_PlaneList[i]->alive)
				return false;
		}
		/*可写通关后的标志之类的*/
		isSuccessful = true;
	}
	else {}
	return false;
}
void initPhysics2() {
	initGunTower();
	Level[1] = true;
}
void initPhysics3() {
	initAI_Plane();
	Level[2] = true;
}

//重置关卡
void resetLevel() {
	//销毁坦克内容
	for (int i = 0; i < 2; i++) {
		if (tankList[i]->alive)gScene->removeActor(*tankList[i]->healthBody);
		gScene->removeActor(*tankList[i]->body);
		//free(tankList[i]);
		//tankList[i] = nullptr;
	}
	for (auto i = exsitBonusList.begin(); i != exsitBonusList.end(); i++) {
		gScene->removeActor(*(*i));
	}
	//销毁炮塔内容
	GunTower.reset();
	//销毁AI飞机内容
	for (int i = 0; i < 4; i++) {
		if (AI_PlaneList[i] != nullptr)
			gScene->removeActor(*AI_PlaneList[i]->body);
		//free(AI_PlaneList[i]);
		AI_PlaneList[i] = nullptr;
	}
	//销毁道具
	Bonus.reset();
	//销毁玩家飞机
	if (Plane_1 != nullptr)
		gScene->removeActor(*Plane_1->body);
	//free(Plane_1);
	Plane_1 = nullptr;

	//重新加载
	initMyAirPlane();
	camera.setTarget(Plane_1);
	initTank();
	initBonus();

	Level[0] = true;
	Level[1] = false;
	Level[2] = false;
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
	sceneDesc.filterShader = testCCDFilterShader2;
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

	//当前为Level0
	Level[2] = true;
	//地面和trigger墙
	testTriggerWall();


	//加载地图
	initMap();

	//初始化导弹管理器
	ManageMissile = new MissileManager();


	//加载玩家飞机
	initMyAirPlane();

	//相机跟踪目标
	//camera.setTarget(player);
	camera.setTarget(Plane_1);
	//camera.setTarget(tankList[1]);
	//camera.setTarget(Plane_AI);


	//加载坦克
	initTank();
	//加载道具
	initBonus();

	//加载4架AI飞机
	initAI_Plane();

}

void beforeStepPhysics() {
	addForceToPartivleSystem(physicsParticleSystemList);
}

//我们所有需要每一帧调用的物理计算放在这里面
void Tick() {
	if (Level[0]) {
		tankAutoMove();
		updateTankInList();

	}
	else if (Level[1]) {
		GunTower.runguntower(Plane_1->body);
		updateGuntowerInList();

	}
	else if (Level[2]) {
		AI_PlaneAutoFly();
		ManageMissile->trackingAllMissile();
		ManageMissile->removeMissile();
	}
	else {}
	Plane_1->manualControlAirPlane4();
	Bonus.runsupply();
	addBonusInList();
	removeActorInList();
	//判断是否需要切换场景
	if (isToChangeLevel()) {
		if (Level[0]) {}
		else if (Level[1]) {
			initPhysics2();
		}
		else if (Level[2]) {
			initPhysics3();
		}
		else {}
	}
	//if(tempList[0]!=nullptr)tempList[0]->autoFlying();
	//Plane_AI->autoFlying();
	//Plane_1->formcloud();

	Plane_1->formmisslecloud();
}


void stepPhysics(bool interactive)
{
	if (!interactive)
		return;

	beforeStepPhysics();
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
	Tick();

	//锁帧
	/*lockFrame_current = clock();//当前时钟
	if ((lockFrame_current - lockFrame_last) < 16) {
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
