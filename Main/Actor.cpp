//#include "Actor.h"
//
//AirPlane::AirPlane() {
//	initTransform = PxTransform(PxVec3(2, 1, -5));
//
//	//����
//	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);
//	this->body = PxCreateDynamic(*gPhysics, initTransform, *shape, 100);
//	body->setName("airPlane");
//	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	gScene->addActor(*body);
//	//������β��
//	PxShape* swing_s = gPhysics->createShape(PxBoxGeometry(0.15, 0.04, 0.3), *gMaterial);
//	PxShape* tail_s = gPhysics->createShape(PxBoxGeometry(0.1, 0.15, 0.04), *gMaterial);
//	PxRigidDynamic* swing1 = PxCreateDynamic(*gPhysics, initTransform, *swing_s, 0.01);
//	PxRigidDynamic* swing2 = PxCreateDynamic(*gPhysics, initTransform, *swing_s, 0.01);
//	PxRigidDynamic* tail = PxCreateDynamic(*gPhysics, initTransform, *tail_s, 0.01);
//	swing1->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	swing1->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	swing2->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	swing2->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	tail->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	tail->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	gScene->addActor(*swing1);
//	gScene->addActor(*swing2);
//	gScene->addActor(*tail);
//	//����
//	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, -0.2)), swing1, PxTransform(PxVec3(0, 0, 0.3)));
//	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, 0.2)), swing2, PxTransform(PxVec3(0, 0, -0.3)));
//	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(-0.3, 0, 0)), tail, PxTransform(PxVec3(0, -0.24, 0)));
//	shape->release();
//	swing_s->release();
//	tail_s->release();
//
//	currentHeadForward = headForward = PxVec3(1.0f, 0.0f, 0.0f);
//	currentBackForward = backForward = PxVec3(0.0f, 1.0f, 0.0f);
//	currentSwingForward = swingForward = PxVec3(0.0f, 0.0f, 1.0f);
//	turningState.resize(5, false);
//	turningState[2] = true;
//	user_data = nullptr;
//
//	emitTransform = PxTransform(-currentBackForward * 10);
//}
//AirPlane::~AirPlane() {
//	free(body);
//	free(user_data);
//	body = nullptr;
//	user_data = nullptr;
//}
//AirPlane::AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body) {
//	currentHeadForward = headForward = head;
//	currentBackForward = backForward = back;
//	currentSwingForward = swingForward = swing;
//	this->body = _body;
//	body->setName("airPlane");
//	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	turningState.resize(5, false);
//	turningState[2] = true;
//	initTransform = PxTransform(PxVec3(0, 0, 0));
//	user_data = nullptr;
//
//	emitTransform = PxTransform(-currentBackForward * 1);
//}
//void AirPlane::controlAirPlane() {
//	//ֱ��
//	if (turningState[2]) {
//		body->setLinearVelocity(veclocity * currentHeadForward);
//		turningState[0] = false;
//		turningState[1] = false;
//		turningState[3] = false;
//		turningState[4] = false;
//	}
//	//��ת
//	else if (turningState[0]) {
//		if (rollingAngel < 45) {
//			rollingAngel += 1;
//			PxQuat rot1(PxPi / 180 * (-1), currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (1), backForward);
//			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else if (rollingAngel < 90) {
//			rollingAngel += 1;
//			PxQuat rot1(PxPi / 180 * (1), currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (1), backForward);
//			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else {
//			turningState[0] = false;
//			turningState[1] = false;
//			turningState[2] = true;
//			turningState[3] = false;
//			turningState[4] = false;
//			rollingAngel = 0;
//			//currentAngel_x = 0;
//			currentAngel_x += 90;
//			currentAngel_x %= 360;
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)));
//			currentHeadForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(headForward);
//			currentBackForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(backForward);
//			currentSwingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(swingForward);
//			body->setLinearVelocity(veclocity * currentHeadForward);
//			cout << "��ת����\n";
//		}
//
//	}
//	//��ת
//	else if (turningState[1]) {
//		if (rollingAngel < 45) {
//			rollingAngel += 1;
//			/*PxQuat rot1(PxPi / 180 * rollingAngel, currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (-rollingAngel * 2 + currentAngel_x), backForward);
//			currentHeadForward = (rot1*rot2).rotate(headForward);
//			currentBackForward = (rot1*rot2).rotate(backForward);
//			currentSwingForward = (rot1*rot2).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
//			PxQuat rot1(PxPi / 180 * (1), currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (-1), backForward);
//			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else if (rollingAngel < 90) {
//			rollingAngel += 1;
//			/*long long angel = 90 - rollingAngel;
//			PxQuat rot1(PxPi / 180 * angel, currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (-90 + currentAngel_x), backForward);
//			currentHeadForward = (rot1*rot2).rotate(headForward);
//			currentBackForward = (rot1*rot2).rotate(backForward);
//			currentSwingForward = (rot1*rot2).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
//			PxQuat rot1(PxPi / 180 * (-1), currentHeadForward);
//			PxQuat rot2(PxPi / 180 * (-1), backForward);
//			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else {
//			turningState[0] = false;
//			turningState[1] = false;
//			turningState[2] = true;
//			turningState[3] = false;
//			turningState[4] = false;
//			rollingAngel = 0;
//			//currentAngel_x = 0;
//			currentAngel_x -= 90;
//			currentAngel_x %= 360;
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)));
//			currentHeadForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(headForward);
//			currentBackForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(backForward);
//			currentSwingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(swingForward);
//			cout << "��ת����\n";
//			//body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//
//	}
//	//����
//	else if (turningState[3]) {
//		//PxVec3 axis_z = currentHeadForward.cross(currentBackForward).getNormalized();
//		if (pitchingAngel < 90) {
//			pitchingAngel += 1;
//			PxQuat rot(PxPi / 180 * (1), currentSwingForward);
//			currentHeadForward = (rot*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else {
//			turningState[0] = false;
//			turningState[1] = false;
//			turningState[2] = true;
//			turningState[3] = false;
//			turningState[4] = false;
//			pitchingAngel = 0;
//			currentAngel_z += 90;
//			currentAngel_z %= 360;
//			cout << "��������\n";
//			//body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//
//	}
//	//����
//	else if (turningState[4]) {
//		if (pitchingAngel < 90) {
//			pitchingAngel += 1;
//			PxQuat rot(PxPi / 180 * (-1), currentSwingForward);
//			currentHeadForward = (rot*body->getGlobalPose().q).rotate(headForward);
//			currentBackForward = (rot*body->getGlobalPose().q).rotate(backForward);
//			currentSwingForward = (rot*body->getGlobalPose().q).rotate(swingForward);
//			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
//			body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//		else {
//			turningState[0] = false;
//			turningState[1] = false;
//			turningState[2] = true;
//			turningState[3] = false;
//			turningState[4] = false;
//			pitchingAngel = 0;
//			currentAngel_z -= 90;
//			currentAngel_z %= 360;
//			cout << "�������\n";
//			//body->setLinearVelocity(veclocity * currentHeadForward);
//		}
//
//	}
//	else {
//
//	}
//	/*angelAirPlane += 1;
//	angelAirPlane = angelAirPlane % 360;
//	PxQuat rot(PxPi / 180 * angelAirPlane, swingForward);
//	currentHeadForward = rot.rotate(headForward);
//	body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot));
//	body->setLinearVelocity(veclocity * currentHeadForward);*/
//
//	/*emitBulletTime++;
//	if (emitBulletTime % 60 == 0) {
//		createDynamic(body->getGlobalPose().transform(PxTransform(currentHeadForward)), PxSphereGeometry(0.1), 25 * currentHeadForward);
//	}*/
//}
//
////������
//void AirPlane::manualControlAirPlane() {
//	currentAngel_x++;
//	//PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0)
//	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
//	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
//	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
//	PxQuat next = body->getGlobalPose().q;
//	//ֱ��
//	if (turningState[2]) {
//		body->setLinearVelocity(veclocity * currentHeadForward);
//	}
//	//��ת
//	if (turningState[0]) {
//		PxQuat rot(PxPi / 180 * (0.1), currentBackForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	//��ת
//	if (turningState[1]) {
//		PxQuat rot(PxPi / 180 * (-0.1), currentBackForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	//����
//	if (turningState[3]) {
//		PxQuat rot(PxPi / 180 * (0.1), currentSwingForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	//����
//	if (turningState[4]) {
//		PxQuat rot(PxPi / 180 * (-0.1), currentSwingForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
//	body->setLinearVelocity(veclocity * currentHeadForward);
//	/*if (currentAngel_x % 100 == 0) {
//		cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
//			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
//			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";
//	}*/
//	/*emitBulletTime++;
//	if (emitBulletTime % 60 == 0) {
//		createDynamic(body->getGlobalPose().transform(PxTransform(currentHeadForward)), PxSphereGeometry(0.1), 25 * currentHeadForward);
//	}*/
//}
//
////��ϼ������ϵ�
//void AirPlane::manualControlAirPlane2() {
//	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
//	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
//	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
//	PxQuat next = body->getGlobalPose().q;
//	//ֱ��
//	if (turningState[2]) {
//		body->setLinearVelocity(veclocity * currentHeadForward);
//	}
//	if (((int)turningState[0] + (int)turningState[1] + (int)turningState[3] + (int)turningState[4]) >= 2) {
//		//����
//		if (turningState[0] && turningState[3]) {
//			PxQuat rot1(PxPi / 180 * (0.1), currentBackForward);
//			PxQuat rot2(PxPi / 180 * (0.1), currentSwingForward);
//			PxQuat rot3(PxPi / 180 * (-0.05), currentHeadForward);
//			next = rot1 * rot2 * rot3 * next;
//		}
//		//����
//		if (turningState[1] && turningState[3]) {
//			PxQuat rot1(PxPi / 180 * (-0.1), currentBackForward);
//			PxQuat rot2(PxPi / 180 * (0.1), currentSwingForward);
//			PxQuat rot3(PxPi / 180 * (0.05), currentHeadForward);
//			next = rot1 * rot2 * rot3 * next;
//		}
//		//����
//		if (turningState[0] && turningState[4]) {
//			PxQuat rot1(PxPi / 180 * (0.1), currentBackForward);
//			PxQuat rot2(PxPi / 180 * (-0.1), currentSwingForward);
//			PxQuat rot3(PxPi / 180 * (-0.05), currentHeadForward);
//			next = rot1 * rot2 * rot3 * next;
//		}
//		//����
//		if (turningState[1] && turningState[4]) {
//			PxQuat rot1(PxPi / 180 * (-0.1), currentBackForward);
//			PxQuat rot2(PxPi / 180 * (-0.1), currentSwingForward);
//			PxQuat rot3(PxPi / 180 * (0.05), currentHeadForward);
//			next = rot1 * rot2 * rot3 * next;
//		}
//	}
//	else {
//		//��ת
//		if (turningState[0]) {
//			PxQuat rot(PxPi / 180 * (0.1), currentBackForward);
//			next = rot * next;
//		}
//		//��ת
//		if (turningState[1]) {
//			PxQuat rot(PxPi / 180 * (-0.1), currentBackForward);
//			next = rot * next;
//		}
//		//����
//		if (turningState[3]) {
//			PxQuat rot(PxPi / 180 * (0.1), currentSwingForward);
//			next = rot * next;
//		}
//		//����
//		if (turningState[4]) {
//			PxQuat rot(PxPi / 180 * (-0.1), currentSwingForward);
//			next = rot * next;
//		}
//	}
//	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
//	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
//	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
//	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
//	body->setLinearVelocity(veclocity * currentHeadForward);
//	if (currentAngel_x % 100 == 0) {
//		/*cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
//			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
//			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";*/
//	}
//}
//
////����ת��ͬʱ���й�ת
//void AirPlane::manualControlAirPlane3() {
//	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
//	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
//	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
//	PxQuat next = body->getGlobalPose().q;
//	//ֱ��
//	if (turningState[2]) {
//		body->setLinearVelocity(veclocity * currentHeadForward);
//	}
//	//��ת
//	if (turningState[0]) {
//		PxQuat rot(PxPi / 180 * (0.1), currentBackForward);
//		PxQuat rot2(PxPi / 180 * (-0.05), currentHeadForward);
//		next = rot2 * rot * next;
//	}
//	//��ת
//	if (turningState[1]) {
//		PxQuat rot(PxPi / 180 * (-0.1), currentBackForward);
//		PxQuat rot2(PxPi / 180 * (0.05), currentHeadForward);
//		next = rot2 * rot * next;
//	}
//	//����
//	if (turningState[3]) {
//		PxQuat rot(PxPi / 180 * (0.1), currentSwingForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	//����
//	if (turningState[4]) {
//		PxQuat rot(PxPi / 180 * (-0.1), currentSwingForward);
//		//next *= rot;
//		next = rot * next;
//	}
//	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
//	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
//	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
//	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
//	body->setLinearVelocity(veclocity * currentHeadForward);
//	/*if (currentAngel_x % 100 == 0) {
//		cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
//			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
//			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";
//	}*/
//}
//void AirPlane::emit() {
//	//���ε�ҩ
//	//PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, body->getGlobalPose().transform(emitTransform), PxSphereGeometry(0.1), *gMaterial, 10.0f);
//	//�����嵯ҩ
//	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, body->getGlobalPose().transform(emitTransform), PxCapsuleGeometry(0.08,0.3), *gMaterial, 10.0f);
//	//���ø�������
//	dynamic->setName("littleBall");
//	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
//	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
//	dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
//	dynamic->setLinearVelocity((veclocity+emitVeclocity)*currentHeadForward);
//	/*UserData data;
//	(data).id = 1;
//	(data).name = "littleBall";
//	(data).health = 10;
//	cout << data.id << endl;*/
//
//	dynamic->userData = new UserData(1, "ab", 10, 100);
//	UserData* temp = reinterpret_cast<UserData*>(dynamic->userData);
//	//cout << temp->id << endl;
//	//cout << a << endl;
//	gScene->addActor(*dynamic);
//}