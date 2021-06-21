#include"Creater.h"


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


vector<PxActor*>		removeActorList;
PxVec3					airPlaneVelocity(0, 0, 0);
long long				angelAirPlane = 0.0;
PxVec3					headForward(1, 0, 0);
extern Camera camera;


PxRigidActor* createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic) {
	PxRigidActor* rigid;
	if (FileUtils::isFileExist(modelPath)) {
		BaseModel* model = new PlainModel(pos, scale, modelPath, shader);
		if (ifStatic) {
			ObjLoader loader(model, MESH_TYPE::TRIANGLE);
			rigid = loader.createStaticActorAndAddToScene(); // ��̬����
		}
		else {
			ObjLoader loader(model, MESH_TYPE::CONVEX);
			rigid = loader.createDynamicActorAndAddToScene(); // ��̬����
		}
		Logger::debug("�������");
	}
	else {
		Logger::error("�ļ������ڣ�" + modelPath);
		return nullptr;
	}
	return rigid;
}




//�Զ���FilterShader�������С������鷢����ײʱΪpairFlags���eCONTACT_DEFAULT
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

//������ײ����
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
//��ײ�ص�����
void module::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	//PX_UNUSED((pairHeader));
	//printf("Enter onContact!\n");
	std::vector<PxContactPairPoint> contactPoints;//�洢ÿһ����������Ϣ
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxRigidActor* actor_0 = (PxRigidActor*)(pairHeader.actors[0]);
		PxRigidActor* actor_1 = (PxRigidActor*)(pairHeader.actors[1]);
		if (actor_0 != NULL && actor_1 != NULL) {
			if (actor_0->getName() == "littleBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "littleBall"&&actor_0->getName() == "box") {
				printf("С�������飡\n");
				removeActorList.push_back((actor_0->getName() == "box" ? actor_0 : actor_1));
			}
			else if (actor_0->getName() == "bigBall"&&actor_1->getName() == "box"
				|| actor_1->getName() == "bigBall"&&actor_0->getName() == "box") {
				printf("���������飡\n");
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

//ɾ��removeActorList�����actor
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
	//���ø�������
	dynamic->setName("littleBall");
	//������ײ�ı�ǩ
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
	player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
	PxVec3 position = player->getGlobalPose().p;
	cout << "position: " << "x: " << position.x << " y: " << position.y << " z: " << position.z << endl;

	//���ø�������
	player->setName("3rdplayer");

	//userdataָ���Լ�
	//dynamic->userData = dynamic;
	//������ײ�ı�ǩ
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
	PxTransform pos(PxVec3(0, 1, -18));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, pos, PxSphereGeometry(1), *gMaterial, 10.0f);
	//���ø�������
	body->setName("bigBall");
	//userdataָ���Լ�
	//body->userData = body;
	//������ײ��ǩ
	setupFiltering(body, FilterGroup::eBIGBALL, FilterGroup::eSTACK);
	body->setLinearVelocity(PxVec3(0, 0, 5));

	gScene->addActor(*body);
	//shape->release();
}
//�ɻ�����
void createAirPlane() {
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);
	PxTransform initPos(PxVec3(2, 1, -15), PxQuat(PxPi / 6, PxVec3(0, 0, 1)));
	PxRigidDynamic* body = PxCreateDynamic(*gPhysics, initPos, *shape, 8);

	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	airPlane = body;
	gScene->addActor(*body);
	shape->release();
}
//�ı�ɻ���̬���ٶȣ�ʹ����Բ���˶�
void changeAirPlaneVelocity() {
	angelAirPlane += 1;
	angelAirPlane = angelAirPlane % 360;
	PxQuat rot(PxPi / 180 * angelAirPlane, PxVec3(0, 0, 1));
	headForward = rot.rotate(PxVec3(1, 0, 0));
	airPlane->setGlobalPose(PxTransform(airPlane->getGlobalPose().p, rot));
	airPlane->setLinearVelocity(5 * headForward);
}

PxRigidDynamic* initPlayer() {

	PxTransform pos(PxVec3(4, 1, 13));
	player = PxCreateDynamic(*gPhysics, pos, PxSphereGeometry(0.5), *gMaterial, 10.0f);
	//���ø�������
	player->setName("player");
	//������ײ��ǩ
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
			//���ø�������
			body->setName("box");
			//userdataָ���Լ�
			//body->userData = body;

			//������ײ�ı�ǩ
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
	PxTransform t1(t.p, PxQuat(glm::acos(cos_tmp) , glmVec3ToPxVec3(-glm::normalize(glm::cross(camera.getFront(), bullet_init_vec3)))));  //������90�ȣ�Ҫת����ǰ��ǰ��
	//std::cout << "xita:" << glm::acos(cos_tmp)  << "\n";
	PxCapsuleGeometry e(0.005, 0.006);
	PxMaterial* me = gPhysics->createMaterial(0.9f, 0.9f, 0.0f);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t1, e, *me, 0.01f);
	//���ø�������
	dynamic->setName(ACTOR_NAME_PLAYER_BULLET.c_str());
	//������ײ�ı�ǩ
	setupFiltering(dynamic, FilterGroup::ePLAYERBULLET, FilterGroup::eSTACK);
	me->release();
	dynamic->setAngularDamping(0.9f);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);


}