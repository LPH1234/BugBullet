#include "Actor.h"

AirPlane::AirPlane() {
	headForward = PxVec3(1.0f, 0.0f, 0.0f);
	backForward = PxVec3(0.0f, 1.0f, 0.0f);
	swingForward = PxVec3(0.0f, 0.0f, 1.0f);
	turningState.resize(5, false);
	rollingAngel = pitchingAngel = 0;
	initTransform = PxTransform(PxVec3(2, 1, -5));

	PxShape* shape = gPhysics->createShape(PxBoxGeometry(0.5, 0.2, 0.2), *gMaterial);
	this->body = PxCreateDynamic(*gPhysics, initTransform, *shape, 100);
	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	gScene->addActor(*body);
	//机翼和尾翼
	PxShape* swing_s = gPhysics->createShape(PxBoxGeometry(0.15, 0.04, 0.3), *gMaterial);
	PxShape* tail_s = gPhysics->createShape(PxBoxGeometry(0.1, 0.15, 0.04), *gMaterial);
	PxRigidDynamic* swing1 = PxCreateDynamic(*gPhysics, initTransform, *swing_s, 0.01);
	PxRigidDynamic* swing2 = PxCreateDynamic(*gPhysics, initTransform, *swing_s, 0.01);
	PxRigidDynamic* tail = PxCreateDynamic(*gPhysics, initTransform, *tail_s, 0.01);
	swing1->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	swing1->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	swing2->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	swing2->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	tail->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	tail->setActorFlag(PxActorFlag::eVISUALIZATION, true);
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
	user_data = nullptr;
}
AirPlane::~AirPlane() {
	free(body);
	free(user_data);
	body = nullptr;
	user_data = nullptr;
}
AirPlane::AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body) {
	headForward = head;
	backForward = back;
	swingForward = swing;
	this->body = _body;
	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);

	turningState.resize(5, false);
	turningState[2] = true;
	rollingAngel = pitchingAngel = 0;
	initTransform = PxTransform(PxVec3(0, 0, 0));
	user_data = nullptr;
}
void AirPlane::controlAirPlane() {
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * headForward);
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
			headForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			PxQuat rot1(PxPi / 180 * (1), headForward);
			PxQuat rot2(PxPi / 180 * (1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
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
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))));
			headForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(1, 0, 0));
			backForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 1, 0));
			swingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 0, 1));
			body->setLinearVelocity(veclocity * headForward);
			cout << "左转结束\n";
		}

	}
	//右转
	else if (turningState[1]) {
		if (rollingAngel < 45) {
			rollingAngel += 1;
			/*PxQuat rot1(PxPi / 180 * rollingAngel, headForward);
			PxQuat rot2(PxPi / 180 * (-rollingAngel * 2 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
			PxQuat rot1(PxPi / 180 * (1), headForward);
			PxQuat rot2(PxPi / 180 * (-1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			/*long long angel = 90 - rollingAngel;
			PxQuat rot1(PxPi / 180 * angel, headForward);
			PxQuat rot2(PxPi / 180 * (-90 + currentAngel_x), PxVec3(0, 1, 0));
			headForward = (rot1*rot2).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
			PxQuat rot1(PxPi / 180 * (-1), headForward);
			PxQuat rot2(PxPi / 180 * (-1), PxVec3(0, 1, 0));
			headForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot1*rot2*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
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
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))));
			headForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(1, 0, 0));
			backForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 1, 0));
			swingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), PxVec3(0, 1, 0))).rotate(PxVec3(0, 0, 1));
			cout << "右转结束\n";
			//body->setLinearVelocity(veclocity * headForward);
		}

	}
	//上仰
	else if (turningState[3]) {
		//PxVec3 axis_z = headForward.cross(backForward).getNormalized();
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (1), swingForward);
			headForward = (rot*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
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
			//body->setLinearVelocity(veclocity * headForward);
		}

	}
	//俯冲
	else if (turningState[4]) {
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (-1), swingForward);
			headForward = (rot*body->getGlobalPose().q).rotate(PxVec3(1, 0, 0));
			backForward = (rot*body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
			swingForward = (rot*body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * headForward);
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
			//body->setLinearVelocity(veclocity * headForward);
		}

	}
	else {

	}
	/*angelAirPlane += 1;
	angelAirPlane = angelAirPlane % 360;
	PxQuat rot(PxPi / 180 * angelAirPlane, PxVec3(0, 0, 1));
	headForward = rot.rotate(PxVec3(1, 0, 0));
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot));
	body->setLinearVelocity(veclocity * headForward);*/

	/*emitBulletTime++;
	if (emitBulletTime % 60 == 0) {
		createDynamic(body->getGlobalPose().transform(PxTransform(headForward)), PxSphereGeometry(0.1), 25 * headForward);
	}*/
}
void AirPlane::manualControlAirPlane() {
	currentAngel_x++;
	//PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0)
	headForward = (body->getGlobalPose().q).rotate(PxVec3(0, 0, 1));
	backForward = (body->getGlobalPose().q).rotate(PxVec3(0, 1, 0));
	swingForward = (body->getGlobalPose().q).rotate(PxVec3(-1, 0, 0));
	PxQuat next = body->getGlobalPose().q;
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * headForward);
	}
	//左转
	if (turningState[0]) {
		PxQuat rot(PxPi / 180 * (0.1), backForward);
		//next *= rot;
		next = rot * next;
	}
	//右转
	if (turningState[1]) {
		PxQuat rot(PxPi / 180 * (-0.1), backForward);
		//next *= rot;
		next = rot * next;
	}
	//上仰
	if (turningState[3]) {
		PxQuat rot(PxPi / 180 * (0.1), swingForward);
		//next *= rot;
		next = rot * next;
	}
	//俯冲
	if (turningState[4]) {
		PxQuat rot(PxPi / 180 * (-0.1), swingForward);
		//next *= rot;
		next = rot * next;
	}
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
	body->setLinearVelocity(veclocity * headForward);
	if (currentAngel_x % 100 == 0) {
		cout << "headForward:" << headForward.x << "\t" << headForward.y << "\t" << headForward.z << "\t" <<
			"backForward:" << backForward.x << "\t" << backForward.y << "\t" << backForward.z << "\t" <<
			"swingForward:" << swingForward.x << "\t" << swingForward.y << "\t" << swingForward.z << "\n";
	}
	/*emitBulletTime++;
	if (emitBulletTime % 60 == 0) {
		createDynamic(body->getGlobalPose().transform(PxTransform(headForward)), PxSphereGeometry(0.1), 25 * headForward);
	}*/
}