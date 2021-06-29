#include"Creater.h"

#include <trng/yarn2.hpp>
#include <trng/uniform_dist.hpp>
#include <trng/normal_dist.hpp>

using namespace std;


PxFoundation*			gFoundation = nullptr;
PxPhysics*				gPhysics = nullptr;
PxCooking*				gCooking = nullptr;
PxDefaultCpuDispatcher* gDispatcher = nullptr;
PxScene*				gScene = nullptr;
PxMaterial*				gMaterial = nullptr;
PxPvd*                  gPvd = nullptr;
PxRigidDynamic*	airPlane = nullptr;
PxRigidDynamic* player = nullptr;
PlainModel *street = nullptr;

PxRigidDynamic* vehicle = nullptr;


extern Shader* envShader;

vector<PxActor*>		removeActorList;
list<PxParticleSystem*> physicsParticleSystemList;
list<BaseParticleCluster*> renderParticleClusterList;
PxVec3					airPlaneVelocity(0, 0, 0);//飞机速度
long long				angelAirPlane = 0.0;
PxVec3					headForward(1, 0, 0);//机头朝向
PxVec3					backForward(0, 1, 0);//机背朝向
PxVec3					swingForward(0, 0, 1);//机翼朝向
vector<bool>			turningState(5, false);//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
long long				rollingAngel = 0, pitchingAngel = 0;//滚转角、俯仰角
extern Camera camera;


PxRigidActor* createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic) {
	PxRigidActor* rigid;
	if (FileUtils::isFileExist(modelPath)) {
		BaseModel* model = new PlainModel(pos, scale, modelPath, shader);
		if (ifStatic) {
			ObjLoader loader(model, MESH_TYPE::TRIANGLE);
			rigid = loader.createStaticActorAndAddToScene(); // 静态刚体
		}
		else {
			ObjLoader loader(model, MESH_TYPE::CONVEX);
			rigid = loader.createDynamicActorAndAddToScene(); // 动态刚体
		}
		Logger::debug("创建完成");
	}
	else {
		Logger::error("文件不存在：" + modelPath);
		return nullptr;
	}
	return rigid;
}



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
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eDETECT_CCD_CONTACT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 == filterData1.word1) || (filterData1.word0 == filterData0.word1)) {
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		cout << "FilterShader!\n";
	}


	return PxFilterFlag::eDEFAULT;

}
//测试将Render.cpp中的FilterShader复制过来
PxFilterFlags testCCDFilterShader2(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eSOLVE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	/*| PxPairFlag::eNOTIFY_TOUCH_FOUND
	| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
	| PxPairFlag::eNOTIFY_CONTACT_POINTS;*/
	// generate contacts for all that were not filtered above
	//pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	cout << "fiterData0.word0:" << filterData0.word0 << "filterData1.word1:" << filterData1.word1
		<< "\tand:" << (filterData0.word0 & filterData1.word1) << "\n";
	if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
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
	printf("调用onContact!\n");
	std::vector<PxContactPairPoint> contactPoints;//存储每一个触碰点信息
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxRigidActor* actor_0 = (PxRigidActor*)(pairHeader.actors[0]);
		PxRigidActor* actor_1 = (PxRigidActor*)(pairHeader.actors[1]);
		if (actor_0 != NULL && actor_1 != NULL) {
			if (actor_0->getName() == "littleBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "box") {
				printf("小球碰方块！\n");
				//removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
				//removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
				removeActorList.push_back(actor_0); removeActorList.push_back(actor_1);
			}
			else if (actor_0->getName() == "bigBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "bigBall"&&actor_0->getName() == "box") {
				printf("大球碰方块！\n");
				//removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
				removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
			}
			else if (actor_0->getName() == "littleBall"&&actor_1->getName() == "map"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "map") {
				removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
			}
			else if (actor_0->getName() == "littleBall"&&actor_1->getName() == "3rdplayer"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "3rdplayer") {
				removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
				PxRigidDynamic* temp1 = reinterpret_cast<PxRigidDynamic*>((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
				UserData* ball = reinterpret_cast<UserData*>(temp1->userData);
				UserData* temp = reinterpret_cast<UserData*>(player->userData);
				if (temp->health - ball->health > 0) {
					temp->health -= ball->health;
					cout << "player - " << ball->health << endl;
				}
				else {
					cout << "player died" << endl;
				}
			}
			else if (actor_0->getName() == "littleBall"&&actor_1->getName() == "Tower"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "Tower") {
				removeActorList.push_back((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
				PxRigidDynamic* temp1 = reinterpret_cast<PxRigidDynamic*>((actor_0->getName() == "littleBall" ? actor_0 : actor_1));
				PxRigidStatic* temp2 = reinterpret_cast<PxRigidStatic*>((actor_0->getName() == "Tower" ? actor_0 : actor_1));
				UserData* ball = reinterpret_cast<UserData*>(temp1->userData);
				TowerData* tower = reinterpret_cast<TowerData*>(temp2->userData);
				if (tower->health - ball->health > 0) {
					tower->health -= ball->health;
					cout << "tower - " << ball->health << endl;
				}
				else {
					cout << "tower died" << endl;
					tower->enable_attacking = false;
				}
			}
			else if (actor_0->getName() == "bullet"&&actor_1->getName() == "map"
				|| actor_1->getName() == "bullet"&&actor_0->getName() == "map") {
				//printf("飞机弹药！\n");
				removeActorList.push_back((actor_0->getName() == "bullet" ? actor_0 : actor_1));
				cout << pairHeader.pairs->contactImpulses << "\n";
			}
			else {}


		}

	}

}

//删除removeActorList里面的actor
void removeActorInList() {
	int n = removeActorList.size();
	for (int i = 0; i < n; i++) {
		gScene->removeActor(*removeActorList[i]);
	}
	removeActorList.clear();
}


PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.8f, 0.8f, 0.0f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *me, 0.1f);
	//设置刚体名称
	dynamic->setName("littleBall");
	//设置碰撞的标签
	setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.5f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	/*UserData data;
	(data).id = 1;
	(data).name = "littleBall";
	(data).health = 10;
	cout << data.id << endl;*/

	dynamic->userData = new UserData(1, "ab", 10, 100);
	UserData* temp = reinterpret_cast<UserData*>(dynamic->userData);
	//cout << temp->id << endl;
	//cout << a << endl;
	gScene->addActor(*dynamic);
	return dynamic;
}

PxRigidDynamic* init3rdplayer(const PxTransform& t, const PxGeometry& geometry) {
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.0f, 0.8f, 0.0f);
	//player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
	//vehicle =createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", envShader, false);
	//player = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(5.0f, 0.0f, 4.0f), glm::vec3(0.05f, 0.05f, 0.05f), "model/vehicle/Alien Animal Updated in Blender-2.81a/animal1.obj", envShader,false));
	player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
	//player->userData = data;
	PxVec3 position = player->getGlobalPose().p;
	cout << "position: " << "x: " << position.x << " y: " << position.y << " z: " << position.z << endl;

	//设置刚体名称
	player->setName("3rdplayer");

	//userdata指向自己
	//dynamic->userData = dynamic;
	//设置碰撞的标签
	setupFiltering(player, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();

	player->userData = new UserData(1, "ab", 100, 100);
	//UserData* temp = reinterpret_cast<UserData*>(player->userData);

	player->setAngularDamping(0.5f);
	player->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//dynamic->setLinearVelocity(velocity);
	gScene->addActor(*player);
	return player;
}


void testFilter() {
	//131.f, 7.0f, 22.0f
	PxRigidDynamic* body1 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(128.f, 1.0f, 24.0f)), PxBoxGeometry(1, 1, 1), *gMaterial, 10.0f);
	PxRigidDynamic* body2 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(131.f, 1.0f, 24.0f)), PxBoxGeometry(1, 1, 1), *gMaterial, 10.0f);
	PxRigidDynamic* body3 = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(134.f, 1.0f, 24.0f)), PxBoxGeometry(1, 1, 1), *gMaterial, 10.0f);

	setupFiltering(body1, FilterGroup::eTESTBOX1, FilterGroup::eTANK);
	setupFiltering(body2, FilterGroup::eTESTBOX2, FilterGroup::eMISILE);
	setupFiltering(body3, FilterGroup::eTESTBOX3, FilterGroup::eTANK | FilterGroup::eMISILE);

	gScene->addActor(*body1);
	gScene->addActor(*body2);
	gScene->addActor(*body3);
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
//飞机刚体
void createAirPlane() {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);
	//PxTransform initPos(PxVec3(2, 1, -15), PxQuat(PxPi / 6, PxVec3(0, 0, 1)));
	PxTransform initPos(PxVec3(10, 1, -5));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, initPos, *shape, 8);
	//vehicle = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/82-koenigsegg-agera/a.obj", envShader, false));

	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	airPlane = body;
	gScene->addActor(*body);

	//机翼和尾翼
	PxShape* swing_s = gPhysics->createShape(PxBoxGeometry(0.15, 0.04, 0.3), *gMaterial);
	PxShape* tail_s = gPhysics->createShape(PxBoxGeometry(0.1, 0.15, 0.04), *gMaterial);
	PxRigidDynamic* swing1 = PxCreateDynamic(*gPhysics, initPos, *swing_s, 0.01);
	PxRigidDynamic* swing2 = PxCreateDynamic(*gPhysics, initPos, *swing_s, 0.01);
	PxRigidDynamic* tail = PxCreateDynamic(*gPhysics, initPos, *tail_s, 0.01);
	gScene->addActor(*swing1);
	gScene->addActor(*swing2);
	gScene->addActor(*tail);

	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, -0.2)), swing1, PxTransform(PxVec3(0, 0, 0.3)));
	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, 0.2)), swing2, PxTransform(PxVec3(0, 0, -0.3)));
	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(-0.3, 0, 0)), tail, PxTransform(PxVec3(0, -0.24, 0)));
	shape->release();
	swing_s->release();
	tail_s->release();

	turningState[2] = true;
}
//获得两个向量之间的夹角
double getAngel(PxVec3 a, PxVec3 b) {
	double cosine = a.getNormalized().dot(b.getNormalized());
	double angel = acos(cosine);
	angel *= 180 / PxPi;
	return angel;
}
//以右手定则判断两向量之间夹角是否小于180
bool lessThan180(PxVec3 a, PxVec3 base) {
	a = a.getNormalized();
	base = base.getNormalized();
	PxVec3 up = base.cross(a);
	double cosine = up.getNormalized().dot(PxVec3(0, 0, 1));
	if (cosine >= 0)return true;
	else return false;
}

int currentAngel_x = 0, currentAngel_z = 0;
int emitBulletTime = 0;
//改变飞机姿态与速度，能够通过上下左右控制俯仰和左右转弯
void changeAirPlaneVelocity() {
	//直行
	if (turningState[2]) {
		airPlane->setLinearVelocity(5 * headForward);
		turningState[0] = false;
		turningState[1] = false;
		turningState[3] = false;
		turningState[4] = false;
	}
	//左转
	else if (turningState[0]) {
		if (rollingAngel < 45) {
			rollingAngel += 1;
			PxQuat rot1(PxPi / 180 * (-1), headForward);
			PxQuat rot2(PxPi / 180 * (1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2*airPlane->getGlobalPose().q));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			long long angel = 90 - rollingAngel;
			PxQuat rot1(PxPi / 180 * (1), headForward);
			PxQuat rot2(PxPi / 180 * (1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2*airPlane->getGlobalPose().q));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else {
			turningState[0] = false;
			turningState[1] = false;
			turningState[2] = true;
			turningState[3] = false;
			turningState[4] = false;
			rollingAngel = 0;
			//currentAngel_x = 0;
			currentAngel_x += 90;
			currentAngel_x %= 360;
			cout << "左转结束\n";
			//airPlane->setLinearVelocity(5 * headForward);
		}

	}
	//右转
	else if (turningState[1]) {
		if (rollingAngel < 45) {
			rollingAngel += 1;
			PxQuat rot1(PxPi / 180 * rollingAngel, headForward);
			PxQuat rot2(PxPi / 180 * (-rollingAngel * 2 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			long long angel = 90 - rollingAngel;
			PxQuat rot1(PxPi / 180 * angel, headForward);
			PxQuat rot2(PxPi / 180 * (-90 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else {
			turningState[0] = false;
			turningState[1] = false;
			turningState[2] = true;
			turningState[3] = false;
			turningState[4] = false;
			rollingAngel = 0;
			//currentAngel_x = 0;
			currentAngel_x -= 90;
			currentAngel_x %= 360;
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))));
			headForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(1, 0, 0));
			backForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 1, 0));
			swingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 0, 1));
			cout << "右转结束\n";
			//airPlane->setLinearVelocity(5 * headForward);
		}

	}
	//上仰
	else if (turningState[3]) {
		//PxVec3 axis_z = headForward.cross(backForward).getNormalized();
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (1), swingForward);
			headForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot*airPlane->getGlobalPose().q));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else {
			turningState[0] = false;
			turningState[1] = false;
			turningState[2] = true;
			turningState[3] = false;
			turningState[4] = false;
			pitchingAngel = 0;
			currentAngel_z += 90;
			currentAngel_z %= 360;
			cout << "上仰结束\n";
			//airPlane->setLinearVelocity(5 * headForward);
		}

	}
	//俯冲
	else if (turningState[4]) {
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (-1), swingForward);
			headForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot*airPlane->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot*airPlane->getGlobalPose().q));
			airPlane->setLinearVelocity(5 * headForward);
		}
		else {
			turningState[0] = false;
			turningState[1] = false;
			turningState[2] = true;
			turningState[3] = false;
			turningState[4] = false;
			pitchingAngel = 0;
			currentAngel_z -= 90;
			currentAngel_z %= 360;
			cout << "俯冲结束\n";
			//airPlane->setLinearVelocity(5 * headForward);
		}

	}
	else {

	}
	/*angelAirPlane += 1;
	angelAirPlane = angelAirPlane % 360;
	PxQuat rot(PxPi / 180 * angelAirPlane, PxVec3(0, 0, 1));
	headForward = rot.rotate(PxVec3(1, 0, 0));
	airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot));
	airPlane->setLinearVelocity(5 * headForward);*/

	emitBulletTime++;
	if (emitBulletTime % 60 == 0) {
		createDynamic(airPlane->getGlobalPose().transform(PxTransform(headForward)), PxSphereGeometry(0.1), 25 * headForward);
	}
}

// fixed, breakable joint
PxJoint* createBreakableFixed(PxRigidActor* a0, const PxTransform& t0, PxRigidActor* a1, const PxTransform& t1)
{
	PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, t0, a1, t1);
	j->setBreakForce(10, 100);
	j->setProjectionLinearTolerance(0.1000000f);
	j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	j->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	j->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);

	return j;
}
void createAbleBreakWall() {
	//PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.1, 0.1, 0.1), *gMaterial);
	vector<vector<PxRigidDynamic*>> allDynamic(5, vector<PxRigidDynamic*>(10));

	PxTransform t(PxVec3(-0.9, 0.1, 0));
	float separation = 0.2f;
	PxVec3 offset(separation / 2, 0, 0);
	PxVec3 offset2(0, separation / 2, 0);
	PxTransform localTm(offset);
	PxRigidDynamic* prev = NULL;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 10; j++) {
			PxRigidDynamic* current = PxCreateDynamic(*gPhysics, t*localTm, PxBoxGeometry(0.1, 0.1, 0.1), *gMaterial, 100);
			allDynamic[i][j] = (current);
			//连接左边
			if (j == 0) {
				prev = NULL;
			}
			createBreakableFixed(prev, prev ? PxTransform(offset) : t.transform(PxTransform(PxVec3(0, 0.2*i, 0))), current, PxTransform(-offset));
			//连接下边
			if (i - 1 < 0) {
				prev = NULL;
			}
			else {
				prev = allDynamic[i - 1][j];
			}
			PxJoint* jj = createBreakableFixed(prev, prev ? PxTransform(offset2) : t.transform(PxTransform(PxVec3(0.2*j, 0, 0))), current, PxTransform(-offset2));
			if (i == 0) {
				jj->setBreakForce(100000, 100000);
			}
			gScene->addActor(*current);
			prev = current;
			localTm.p.x += separation;
		}
		localTm.p.x = separation / 2;
		localTm.p.y += separation;
	}
}

void createBreakableWall() {
	PxMaterial* wallMaterial = gPhysics->createMaterial(0.7f, 0.65f, 0.1f);
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(2, 2, 2), *wallMaterial);
	vector<vector<PxRigidDynamic*>> Wall(10, vector<PxRigidDynamic*>(5));
	PxReal x = 0.0, y = 2.0, z = 0.0;
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 5; j++) {
			PxTransform pos(PxVec3(x + (j - 2) * 4, y, z));
			PxRigidDynamic* body = gPhysics->createRigidDynamic(pos);
			setupFiltering(body, FilterGroup::eWALL, FilterGroup::eMISILE | FilterGroup::eWALL);
			Wall[i][j] = body;
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 0.1f);
			gScene->addActor(*body);
		}
		y += 4;
	}
	for (int j = 0; j < 10; j++) {
		for (int i = 1; i <= 4; i++) {
			PxRigidDynamic* a0 = Wall[j][i - 1];
			PxRigidDynamic* a1 = Wall[j][i];
			PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, PxTransform(PxVec3(2, 0, 0)), a1, PxTransform(PxVec3(-2, 0, 0)));
			j->setBreakForce(1000.f - abs((i - 2.5) * 100), 100000.f);
		}
	}
	for (int i = 1; i <= 9; i++) {
		PxRigidDynamic* a0 = Wall[i - 1][2];
		PxRigidDynamic* a1 = Wall[i][2];
		PxFixedJoint* j = PxFixedJointCreate(*gPhysics, a0, PxTransform(PxVec3(0, 2, 0)), a1, PxTransform(PxVec3(0, -2, 0)));
		j->setBreakForce(9000.f - abs((i - 1) * 1000), 100000.f);
	}

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

// h:21.6   r:2.25   half_height:8.55
//14.7   2.4  body
//7.0    1.6  head
void createBullet(const PxTransform& t, const PxVec3& velocity) {

	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxQuat q1 = t.q + PxQuat(PxPi / 180 * 90, glmVec3ToPxVec3(camera.getRight()));
	//PxTransform t1(t.p, q1);
	glm::vec3 bullet_init_vec3(1.f, 0.f, 0.f);
	float cos_tmp = glm::dot(camera.getFront(), bullet_init_vec3) / getVec3Length(camera.getFront()) / getVec3Length(bullet_init_vec3);
	PxTransform t1(t.p, PxQuat(glm::acos(cos_tmp), glmVec3ToPxVec3(-glm::normalize(glm::cross(camera.getFront(), bullet_init_vec3)))));  //不能是90度，要转到当前的前方
	//std::cout << "xita:" << glm::acos(cos_tmp)  << "\n";
	PxCapsuleGeometry e(0.005, 0.006);
	PxMaterial* me = gPhysics->createMaterial(0.9f, 0.9f, 0.0f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t1, e, *me, 0.01f);
	//设置刚体名称
	dynamic->setName(ACTOR_NAME_PLAYER_BULLET.c_str());
	//设置碰撞的标签
	setupFiltering(dynamic, FilterGroup::ePLAYERBULLET, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.9f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);


}


//void createshell(const PxTransform& t, const PxVec3& velocity) {
//	if (!t.isValid()) {
//		Logger::error("error:");
//	}
//	PxQuat q1 = t.q + PxQuat(PxPi / 180 * 90, glmVec3ToPxVec3(camera.getRight()));
//	//PxTransform t1(t.p, q1);
//	glm::vec3 bullet_init_vec3(1.f, 0.f, 0.f);
//	float cos_tmp = glm::dot(camera.getFront(), bullet_init_vec3) / getVec3Length(camera.getFront()) / getVec3Length(bullet_init_vec3);
//	PxTransform t1(t.p, PxQuat(glm::acos(cos_tmp), glmVec3ToPxVec3(-glm::normalize(glm::cross(camera.getFront(), bullet_init_vec3)))));  //不能是90度，要转到当前的前方
//	//std::cout << "xita:" << glm::acos(cos_tmp)  << "\n";
//	PxCapsuleGeometry e(0.05, 0.06);
//	PxMaterial* me = gPhysics->createMaterial(0.9f, 0.9f, 0.0f);
//	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t1, e, *me, 0.01f);
//	//设置刚体名称
//	dynamic->setName(ACTOR_NAME_PLAYER_BULLET.c_str());
//	//设置碰撞的标签
//	setupFiltering(dynamic, FilterGroup::ePLAYERBULLET, FilterGroup::eSTACK);
//	me->release();
//	dynamic->setAngularDamping(0.9f);
//	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
//	dynamic->setLinearVelocity(velocity);
//	gScene->addActor(*dynamic);
//}
void createPointParticles(int numParticles, bool perOffset, BaseParticle* renderModel, PxVec3 initPos, bool ifDisperse, double maxDisperseRadius, bool ifRandomV, double maxRandomV, int deleteDelaySec, int fadeDelaySec, PxVec3 velocity, PxVec3 force) {

	PxParticleSystem* ps = gPhysics->createParticleSystem(numParticles, perOffset);;

	// create particle system in PhysX SDK
	PxParticleExt::IndexPool *myindexpool = PxParticleExt::createIndexPool(1 * numParticles);
	//Create buffers that are the size of the particles to be addded
	PxU32 *newAppParticleIndices = new PxU32[numParticles];
	PxVec3 *newAppParticlePositions = new PxVec3[numParticles];
	PxVec3 *newAppParticleVelocities = new PxVec3[numParticles];
	PxVec3 *newAppParticleforces = new PxVec3[numParticles];
	PxParticleCreationData particleCreationData;
	ps->setDamping(1.f);
	ps->setRestitution(1.f);
	ps->setDynamicFriction(1.f);
	ps->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
	ps->setParticleReadDataFlag(PxParticleReadDataFlag::ePOSITION_BUFFER, true);

	trng::yarn2 R;
	trng::uniform_dist<> random_v(-maxRandomV, maxRandomV);
	trng::normal_dist<> random_p(0, maxDisperseRadius);

	for (int i = 0; i < numParticles; i++)
	{
		//Only positions and velocities are given an initial value
		//Indices zill be autogenerated by the indexpool after the loop
		if (ifDisperse) {
			newAppParticlePositions[i].x = initPos.x + random_p(R); // 使初始化粒子散开
			newAppParticlePositions[i].y = initPos.y + random_p(R);
			newAppParticlePositions[i].z = initPos.z + random_p(R);
		}
		else {
			newAppParticlePositions[i].x = initPos.x; // 初始化粒子在一个点上
			newAppParticlePositions[i].y = initPos.y;
			newAppParticlePositions[i].z = initPos.z;
		}


		if (ifRandomV) { // 初始化粒子具有随机速度
			newAppParticleVelocities[i].x = random_v(R);
			newAppParticleVelocities[i].y = random_v(R);
			newAppParticleVelocities[i].z = random_v(R);
		}
		else { // 初始化粒子具有指定速度
			newAppParticleVelocities[i].x = velocity.x;
			newAppParticleVelocities[i].y = velocity.y;
			newAppParticleVelocities[i].z = velocity.z;
		}

		newAppParticleforces[i].x = force.x;
		newAppParticleforces[i].y = force.y;
		newAppParticleforces[i].z = force.z;
	}
	particleCreationData.numParticles = numParticles;
	PxU32 numalloc = myindexpool->allocateIndices(numParticles, PxStrideIterator<PxU32>(newAppParticleIndices));
	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(newAppParticleIndices);
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(newAppParticlePositions);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(newAppParticleVelocities);
	if (particleCreationData.isValid())
	{
		if (ps->createParticles(particleCreationData)) {
			ParticleSystemData* data = new ParticleSystemData;
			data->hasForce = force.x != 0.f || force.y != 0.f || force.z != 0.f;
			data->numParticles = numParticles;
			data->deleteDelaySec = deleteDelaySec;
			data->createTime = clock();
			data->fadeDelaySec = fadeDelaySec;
			renderModel->setParticleSystem(ps);
			data->renderModel = renderModel;
			data->newAppParticleforces = newAppParticleforces;
			data->newAppParticleIndices = newAppParticleIndices;
			data->indexPool = myindexpool;
			for (int i = 0; i < numParticles; i++, particleCreationData.indexBuffer++)
				newAppParticleIndices[i] = *particleCreationData.indexBuffer;
			ps->userData = (void*)data;
			cout << "创建粒子成功\n";
		}
		else {
			ps->release();
			ps = nullptr;
			delete renderModel;
			delete newAppParticleforces;
			delete newAppParticleIndices;
			cout << "创建粒子失败\n";
		}
	}

	if (ps) {
		gScene->addActor(*ps);
		physicsParticleSystemList.push_back(ps);
	}
	//Cleanup
	delete newAppParticlePositions;
	delete newAppParticleVelocities;
}

void createSmokeParticles(int numParticles, bool perOffset, BaseParticle* renderModel, PxVec3 initPos, bool ifDisperse, double maxDisperseRadius, bool ifRandomV, double maxRandomV, int deleteDelaySec, int fadeDelaySec, PxVec3 velocity, PxVec3 force) {

	PxParticleSystem* ps = gPhysics->createParticleSystem(numParticles, perOffset);;

	// create particle system in PhysX SDK
	PxParticleExt::IndexPool *myindexpool = PxParticleExt::createIndexPool(1 * numParticles);
	//Create buffers that are the size of the particles to be addded
	PxU32 *newAppParticleIndices = new PxU32[numParticles];
	PxVec3 *newAppParticlePositions = new PxVec3[numParticles];
	PxVec3 *newAppParticleVelocities = new PxVec3[numParticles];
	PxVec3 *newAppParticleforces = new PxVec3[numParticles];
	PxVec4 *axisAndAngle = new PxVec4[numParticles];
	PxParticleCreationData particleCreationData;
	ps->setDamping(1.f);
	ps->setRestitution(1.f);
	ps->setDynamicFriction(1.f);
	ps->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
	ps->setParticleReadDataFlag(PxParticleReadDataFlag::ePOSITION_BUFFER, true);

	trng::yarn2 R(clock());
	trng::uniform_dist<> random_v(-maxRandomV, maxRandomV);
	trng::uniform_dist<> random_p(-maxDisperseRadius, maxDisperseRadius);
	trng::normal_dist<> random_axis(-1, 1);
	trng::normal_dist<> random_angle(-180, 180);
	srand(clock());

	for (int i = 0; i < numParticles; i++)
	{
		//Only positions and velocities are given an initial value
		//Indices zill be autogenerated by the indexpool after the loop
		if (ifDisperse) {
			newAppParticlePositions[i].x = initPos.x + random_p(R); // 使初始化粒子散开(int(maxDisperseRadius))
			newAppParticlePositions[i].y = initPos.y + random_p(R);
			newAppParticlePositions[i].z = initPos.z + random_p(R);
		}
		else {
			newAppParticlePositions[i].x = initPos.x; // 初始化粒子在一个点上
			newAppParticlePositions[i].y = initPos.y;
			newAppParticlePositions[i].z = initPos.z;
		}


		if (ifRandomV) { // 初始化粒子具有随机速度
			newAppParticleVelocities[i].x = random_v(R);
			newAppParticleVelocities[i].y = random_v(R);
			newAppParticleVelocities[i].z = random_v(R);
		}
		else { // 初始化粒子具有指定速度
			newAppParticleVelocities[i].x = velocity.x;
			newAppParticleVelocities[i].y = velocity.y;
			newAppParticleVelocities[i].z = velocity.z;
		}

		newAppParticleforces[i].x = force.x;
		newAppParticleforces[i].y = force.y;
		newAppParticleforces[i].z = force.z;

		axisAndAngle[i].x = random_axis(R);
		axisAndAngle[i].y = random_axis(R);
		axisAndAngle[i].z = random_axis(R);
		axisAndAngle[i].w = random_angle(R);
	}
	particleCreationData.numParticles = numParticles;
	PxU32 numalloc = myindexpool->allocateIndices(numParticles, PxStrideIterator<PxU32>(newAppParticleIndices));
	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(newAppParticleIndices);
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(newAppParticlePositions);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(newAppParticleVelocities);
	if (particleCreationData.isValid())
	{
		if (ps->createParticles(particleCreationData)) {
			ParticleSystemData* data = new ParticleSystemData;
			data->hasForce = force.x != 0.f || force.y != 0.f || force.z != 0.f;
			data->numParticles = numParticles;
			data->deleteDelaySec = deleteDelaySec;
			data->createTime = clock();
			data->fadeDelaySec = fadeDelaySec;
			renderModel->setParticleSystem(ps);
			data->renderModel = renderModel;
			data->newAppParticleforces = newAppParticleforces;
			data->newAppParticleIndices = newAppParticleIndices;
			data->indexPool = myindexpool;
			data->axisAndAngle = axisAndAngle;
			for (int i = 0; i < numParticles; i++, particleCreationData.indexBuffer++)
				newAppParticleIndices[i] = *particleCreationData.indexBuffer;
			ps->userData = (void*)data;
			cout << "创建粒子成功\n";
			gScene->addActor(*ps);
			physicsParticleSystemList.push_back(ps);
		}
		else {
			myindexpool->freeIndices();
			myindexpool->release();
			ps->release();
			ps = nullptr;
			delete renderModel;
			delete newAppParticleforces;
			delete newAppParticleIndices;
			delete axisAndAngle;
			cout << "创建粒子失败\n";
		}
	}

	//Cleanup
	delete newAppParticlePositions;
	delete newAppParticleVelocities;
}

void addForceToPartivleSystem(list<PxParticleSystem*>& particleSystemList) {

	list<PxParticleSystem*>::iterator it; //声明一个迭代器

	for (it = particleSystemList.begin(); it != particleSystemList.end(); it++) {
		PxParticleSystem* ps = *it;
		ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);

		ps->addForces(data->numParticles, PxStrideIterator<const PxU32>(data->newAppParticleIndices), PxStrideIterator<const PxVec3>(data->newAppParticleforces), PxForceMode::eACCELERATION);
	}

}

float* createUniformRandomFloatArray(int num, float bottom, float up) {
	float* rt = new float[num];

	trng::yarn2 R(clock());
	trng::uniform_dist<> random_u(bottom, up);
	for (int i = 0; i < num; i++)
	{
		rt[i] = random_u(R);
	}
	return rt;
}

float* createNormalRandomFloatArray(int num, float arg1, float arg2, float* rt) {
	std::cout << "allocate:" <<num  << " \n";
	//float* rt = new float[num];
	std::cout << "allocate over\n";

	trng::yarn2 R(clock());
	trng::normal_dist<> random_n(arg1, arg2); // 均值、标准差
	for (int i = 0; i < num; i++)
	{
		rt[i] = random_n(R);
	}
	return rt;
}