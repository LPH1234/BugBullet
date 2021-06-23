#include"Creater.h"
#define TURN_LEFT 0;
#define TURN_RIGHT 1;
#define STRAIGHT 2;
#define GO_UP 3;
#define GO_dOWN 4;

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
PxRigidDynamic* guntower_1 = nullptr;
PxRigidDynamic* guntower_2 = nullptr;
PxRigidDynamic* guntower_3 = nullptr;
PxRigidDynamic* guntower_4 = nullptr;

extern Shader* envShader;

vector<PxActor*>		removeActorList;
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
				if (temp->health-ball->health > 0) {
					temp->health -= ball->health;
					cout << "player - " << ball->health << endl;
				}
				else {
					cout << "player died" << endl;
				}
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
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *me, 10.0f);
	//设置刚体名称
	dynamic->setName("littleBall");
	//设置碰撞的标签
	setupFiltering(dynamic, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.5f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	/*UserData data;
	(data).id = 1;
	(data).name = "littleBall";
	(data).health = 10;
	cout << data.id << endl;*/

	dynamic->userData = new UserData(1,"ab",10,100);
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

	player->userData = new UserData(1, "ab", 100,100);
	//UserData* temp = reinterpret_cast<UserData*>(player->userData);

	player->setAngularDamping(0.5f);
	player->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//dynamic->setLinearVelocity(velocity);
	gScene->addActor(*player);
	return player;
}

PxRigidDynamic* initvehicle(const PxTransform& t, const PxGeometry& geometry) {
	if (!t.isValid()) {
		Logger::error("error:");
	}
	PxMaterial* me = gPhysics->createMaterial(0.0f, 0.8f, 0.0f);
	//player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
	vehicle = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/82-koenigsegg-agera/a.obj", envShader, false));

	PxVec3 position = vehicle->getGlobalPose().p;
	//cout << "position: " << "x: " << position.x << " y: " << position.y << " z: " << position.z << endl;
	cout << "create vehicle" << endl;
	//设置刚体名称
	vehicle->setName("vehicle");

	//userdata指向自己
	//dynamic->userData = dynamic;
	//设置碰撞的标签
	setupFiltering(vehicle, FilterGroup::eBALL, FilterGroup::eSTACK);
	me->release();

	vehicle->setAngularDamping(0.5f);
	vehicle->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	//dynamic->setLinearVelocity(velocity);
	gScene->addActor(*vehicle);
	return vehicle;
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
	else if(turningState[0]){
		if (rollingAngel < 45) {
			rollingAngel += 1;
			/*PxQuat rot1(-PxPi / 180 * rollingAngel, headForward);
			PxQuat rot2(PxPi / 180 * (rollingAngel * 2 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2));*/
			PxQuat rot1(PxPi / 180 * (-1), headForward);
			PxQuat rot2(PxPi / 180 * (1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*airPlane->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2*airPlane->getGlobalPose().q));
			airPlane->setLinearVelocity(5 * headForward);
			//cout << getAngel(headForward, PxVec3(1, 0, 0)) << "\n";
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			long long angel = 90 - rollingAngel;
			/*PxQuat rot1(-PxPi / 180 * angel, headForward);
			PxQuat rot2(PxPi / 180 * (90 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot1*rot2));*/
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
			/*airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, PxQuat(PxPi/2*(currentAngel_x/90),PxVec3(0,1,0))));
			headForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(1, 0, 0));
			backForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 1, 0));
			swingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 0, 1));*/
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
			/*PxQuat rot(PxPi / 180 * (pitchingAngel + currentAngel_z), swingForward);
			headForward = (rot).rotate(PxVec3(1, 0, 0));
			backForward = (rot).rotate(PxVec3(0, 1, 0));
			swingForward = (rot).rotate(PxVec3(0, 0, 1));
			airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot));*/
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
		/*PxQuat rot(PxPi / 180 * (pitchingAngel + currentAngel_z), swingForward);
		headForward = (rot).rotate(PxVec3(1, 0, 0));
		backForward = (rot).rotate(PxVec3(0, 1, 0));
		swingForward = (rot).rotate(PxVec3(0, 0, 1));
		airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot));*/
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
}

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
	float cos_tmp = glm::dot(camera.getFront(), bullet_init_vec3)/getVec3Length(camera.getFront())/getVec3Length(bullet_init_vec3);
	PxTransform t1(t.p, PxQuat(glm::acos(cos_tmp) , glmVec3ToPxVec3(-glm::normalize(glm::cross(camera.getFront(), bullet_init_vec3)))));  //不能是90度，要转到当前的前方
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

PxVec3 guntower::initguntower(glm::vec3 pos) {
	//glm::vec3 pos1(5.0f, 5.0f, 0.0f);
	glm::vec3 pos1(pos.x, pos.y-0.75f, pos.z);
	
	guntower_1 = reinterpret_cast<PxRigidDynamic*>(createModel(pos1, glm::vec3(0.05f, 0.05f, 0.05f), "model/vehicle/tower/c.obj", envShader));
	PxVec3 mPos; glmVec3ToPxVec3(pos, mPos);
	//PxTransform mDir = PxTransform(target->getGlobalPose().p-mPos);
	//PxVec3 mDir = (target->getGlobalPose().p - mPos);
	//autoattack(PxTransform(mPos), mDir);
	//guntower::towerpos = mPos;

	//guntower::towerpos_list.push_back(mPos);
	//guntower::timer_list.push_back(0);
	return mPos;
}
void guntower::initlist(vector<glm::vec3> pos_list) {
	for (int i = 0; i < pos_list.size(); i++) {
		PxVec3 e =initguntower(pos_list[i]);
		guntower::towerpos_list.push_back(e);
		guntower::timer_list.push_back(0);
	}
}
void guntower::autoattack(PxRigidDynamic* target,PxVec3 pos) {
	PxVec3 velocity = (target->getGlobalPose().p - pos);
	createDynamic(PxTransform(pos), PxSphereGeometry(0.1f), velocity);
	cout << "gunshot" << endl;
}
void guntower::runguntower(PxRigidDynamic* target) {
	//clock_t timer_now = clock();
	for (int i = 0; i < towerpos_list.size(); i++) {
		PxVec3 e = towerpos_list[i];
		clock_t timer_now = clock();
		if (timer_now - timer_list[i] > 1000) {
			autoattack(target, e);
			timer_list[i] = timer_now;
		}
	}
	
	/*PxVec3 e= guntower::towerpos;
	clock_t timer_now = clock();
	if (timer_now - timer_last > 1000) {
		autoattack(target, e);
		timer_last = timer_now;
	}*/
	
}