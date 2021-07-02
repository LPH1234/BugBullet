#include "Characters.h"
#include "../Render/Camera.h"

extern void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
extern Shader* cloudShader;

AirPlane::AirPlane() :BaseCharacter(nullptr) {
	initTransform = PxTransform(PxVec3(2, 1, -5));

	//机身
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
	//连接
	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, -0.2)), swing1, PxTransform(PxVec3(0, 0, 0.3)));
	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(0, 0, 0.2)), swing2, PxTransform(PxVec3(0, 0, -0.3)));
	PxFixedJointCreate(*gPhysics, body, PxTransform(PxVec3(-0.3, 0, 0)), tail, PxTransform(PxVec3(0, -0.24, 0)));
	shape->release();
	swing_s->release();
	tail_s->release();

	currentHeadForward = headForward = PxVec3(1.0f, 0.0f, 0.0f);
	currentBackForward = backForward = PxVec3(0.0f, 1.0f, 0.0f);
	currentSwingForward = swingForward = PxVec3(0.0f, 0.0f, 1.0f);
	turningState.resize(5, false);
	turningState[2] = true;
	turningState2.resize(7);
	turningState2[0] = true;
	user_data = nullptr;

	emitTransform = PxTransform(-currentBackForward * 10);
}
AirPlane::~AirPlane() {
	free(body);
	free(user_data);
	body = nullptr;
	user_data = nullptr;
}
AirPlane::AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body) :BaseCharacter(_body) {
	currentHeadForward = headForward = head;
	currentBackForward = backForward = back;
	currentSwingForward = swingForward = swing;
	PxQuat emitRot(PxPi / 180 * (-10), currentSwingForward);
	emitDirection = emitRot.rotate(currentHeadForward).getNormalized();
	this->body = _body;
	setupFiltering(this->body, FilterGroup::eAIRPLANE, FilterGroup::eMAP | FilterGroup::ePLAYERBULLET);
	body->setName("airPlane");
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	body->userData = new UserData(this ,1, "plane", DATATYPE::ACTOR_TYPE::PLANE);
	setupFiltering(body, FilterGroup::ePlayer, FilterGroup::eMISILE);
	turningState.resize(5, false);
	turningState[2] = true;
	turningState2.resize(7);
	turningState2[0] = true;
	initTransform = PxTransform(PxVec3(0, 0, 0));
	user_data = nullptr;

	emitTransform = PxTransform(-currentBackForward * 1);
}

void AirPlane::controlAirPlane() {
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * currentHeadForward);
		turningState[0] = false;
		turningState[1] = false;
		turningState[3] = false;
		turningState[4] = false;
	}
	//左转
	else if (turningState[0]) {
		if (rollingAngel < 45) {
			rollingAngel += 1;
			PxQuat rot1(PxPi / 180 * (-1), currentHeadForward);
			PxQuat rot2(PxPi / 180 * (1), backForward);
			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			PxQuat rot1(PxPi / 180 * (1), currentHeadForward);
			PxQuat rot2(PxPi / 180 * (1), backForward);
			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
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
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)));
			currentHeadForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(headForward);
			currentBackForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(backForward);
			currentSwingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(swingForward);
			body->setLinearVelocity(veclocity * currentHeadForward);
			cout << "左转结束\n";
		}

	}
	//右转
	else if (turningState[1]) {
		if (rollingAngel < 45) {
			rollingAngel += 1;
			/*PxQuat rot1(PxPi / 180 * rollingAngel, currentHeadForward);
			PxQuat rot2(PxPi / 180 * (-rollingAngel * 2 + currentAngel_x), backForward);
			currentHeadForward = (rot1*rot2).rotate(headForward);
			currentBackForward = (rot1*rot2).rotate(backForward);
			currentSwingForward = (rot1*rot2).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
			PxQuat rot1(PxPi / 180 * (1), currentHeadForward);
			PxQuat rot2(PxPi / 180 * (-1), backForward);
			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
		}
		else if (rollingAngel < 90) {
			rollingAngel += 1;
			/*long long angel = 90 - rollingAngel;
			PxQuat rot1(PxPi / 180 * angel, currentHeadForward);
			PxQuat rot2(PxPi / 180 * (-90 + currentAngel_x), backForward);
			currentHeadForward = (rot1*rot2).rotate(headForward);
			currentBackForward = (rot1*rot2).rotate(backForward);
			currentSwingForward = (rot1*rot2).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2));*/
			PxQuat rot1(PxPi / 180 * (-1), currentHeadForward);
			PxQuat rot2(PxPi / 180 * (-1), backForward);
			currentHeadForward = (rot1*rot2*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot1*rot2*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot1*rot2*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot1*rot2*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
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
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)));
			currentHeadForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(headForward);
			currentBackForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(backForward);
			currentSwingForward = (PxQuat(PxPi / 2 * (currentAngel_x / 90), backForward)).rotate(swingForward);
			cout << "右转结束\n";
			//body->setLinearVelocity(veclocity * currentHeadForward);
		}

	}
	//上仰
	else if (turningState[3]) {
		//PxVec3 axis_z = currentHeadForward.cross(currentBackForward).getNormalized();
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (1), currentSwingForward);
			currentHeadForward = (rot*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
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
			//body->setLinearVelocity(veclocity * currentHeadForward);
		}

	}
	//俯冲
	else if (turningState[4]) {
		if (pitchingAngel < 90) {
			pitchingAngel += 1;
			PxQuat rot(PxPi / 180 * (-1), currentSwingForward);
			currentHeadForward = (rot*body->getGlobalPose().q).rotate(headForward);
			currentBackForward = (rot*body->getGlobalPose().q).rotate(backForward);
			currentSwingForward = (rot*body->getGlobalPose().q).rotate(swingForward);
			body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot*body->getGlobalPose().q));
			body->setLinearVelocity(veclocity * currentHeadForward);
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
			//body->setLinearVelocity(veclocity * currentHeadForward);
		}

	}
	else {

	}
	/*angelAirPlane += 1;
	angelAirPlane = angelAirPlane % 360;
	PxQuat rot(PxPi / 180 * angelAirPlane, swingForward);
	currentHeadForward = rot.rotate(headForward);
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, rot));
	body->setLinearVelocity(veclocity * currentHeadForward);*/

	/*emitBulletTime++;
	if (emitBulletTime % 60 == 0) {
		createDynamic(body->getGlobalPose().transform(PxTransform(currentHeadForward)), PxSphereGeometry(0.1), 25 * currentHeadForward);
	}*/
}
//左右转的同时进行滚转
void AirPlane::manualControlAirPlane() {
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	PxQuat next = body->getGlobalPose().q;
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * currentHeadForward);
	}
	//左转
	if (turningState[0]) {
		PxQuat rot(PxPi / 180 * (0.1*turningSpeed), currentBackForward);
		PxQuat rot2(PxPi / 180 * (-0.05*turningSpeed), currentHeadForward);
		next = rot2 * rot * next;
	}
	//右转
	if (turningState[1]) {
		PxQuat rot(PxPi / 180 * (-0.1*turningSpeed), currentBackForward);
		PxQuat rot2(PxPi / 180 * (0.05*turningSpeed), currentHeadForward);
		next = rot2 * rot * next;
	}
	//上仰
	if (turningState[3]) {
		PxQuat rot(PxPi / 180 * (0.1*turningSpeed), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	//俯冲
	if (turningState[4]) {
		PxQuat rot(PxPi / 180 * (-0.1*turningSpeed), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	body->setLinearVelocity(veclocity * currentHeadForward);
	/*if (currentAngel_x % 100 == 0) {
		cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";
	}*/
}

//单按键
void AirPlane::manualControlAirPlane2() {
	currentAngel_x++;
	//PxVec3(0, 0, 1), PxVec3(0, 1, 0), PxVec3(-1, 0, 0)
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	PxQuat next = body->getGlobalPose().q;
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * currentHeadForward);
	}
	//左转
	if (turningState[0]) {
		PxQuat rot(PxPi / 180 * (0.1), currentBackForward);
		//next *= rot;
		next = rot * next;
	}
	//右转
	if (turningState[1]) {
		PxQuat rot(PxPi / 180 * (-0.1), currentBackForward);
		//next *= rot;
		next = rot * next;
	}
	//上仰
	if (turningState[3]) {
		PxQuat rot(PxPi / 180 * (0.1), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	//俯冲
	if (turningState[4]) {
		PxQuat rot(PxPi / 180 * (-0.1), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
	body->setLinearVelocity(veclocity * currentHeadForward);
	/*if (currentAngel_x % 100 == 0) {
		cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";
	}*/
	/*emitBulletTime++;
	if (emitBulletTime % 60 == 0) {
		createDynamic(body->getGlobalPose().transform(PxTransform(currentHeadForward)), PxSphereGeometry(0.1), 25 * currentHeadForward);
	}*/
}

//组合键，左上等
void AirPlane::manualControlAirPlane3() {
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	PxQuat next = body->getGlobalPose().q;
	//直行
	if (turningState[2]) {
		body->setLinearVelocity(veclocity * currentHeadForward);
	}
	if (((int)turningState[0] + (int)turningState[1] + (int)turningState[3] + (int)turningState[4]) >= 2) {
		//左上
		if (turningState[0] && turningState[3]) {
			PxQuat rot1(PxPi / 180 * (0.1), currentBackForward);
			PxQuat rot2(PxPi / 180 * (0.1), currentSwingForward);
			PxQuat rot3(PxPi / 180 * (-0.05), currentHeadForward);
			next = rot1 * rot2 * rot3 * next;
		}
		//右上
		if (turningState[1] && turningState[3]) {
			PxQuat rot1(PxPi / 180 * (-0.1), currentBackForward);
			PxQuat rot2(PxPi / 180 * (0.1), currentSwingForward);
			PxQuat rot3(PxPi / 180 * (0.05), currentHeadForward);
			next = rot1 * rot2 * rot3 * next;
		}
		//左下
		if (turningState[0] && turningState[4]) {
			PxQuat rot1(PxPi / 180 * (0.1), currentBackForward);
			PxQuat rot2(PxPi / 180 * (-0.1), currentSwingForward);
			PxQuat rot3(PxPi / 180 * (-0.05), currentHeadForward);
			next = rot1 * rot2 * rot3 * next;
		}
		//右下
		if (turningState[1] && turningState[4]) {
			PxQuat rot1(PxPi / 180 * (-0.1), currentBackForward);
			PxQuat rot2(PxPi / 180 * (-0.1), currentSwingForward);
			PxQuat rot3(PxPi / 180 * (0.05), currentHeadForward);
			next = rot1 * rot2 * rot3 * next;
		}
	}
	else {
		//左转
		if (turningState[0]) {
			PxQuat rot(PxPi / 180 * (0.1), currentBackForward);
			next = rot * next;
		}
		//右转
		if (turningState[1]) {
			PxQuat rot(PxPi / 180 * (-0.1), currentBackForward);
			next = rot * next;
		}
		//上仰
		if (turningState[3]) {
			PxQuat rot(PxPi / 180 * (0.1), currentSwingForward);
			next = rot * next;
		}
		//俯冲
		if (turningState[4]) {
			PxQuat rot(PxPi / 180 * (-0.1), currentSwingForward);
			next = rot * next;
		}
	}
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	body->setLinearVelocity(veclocity * currentHeadForward);
	if (currentAngel_x % 100 == 0) {
		/*cout << "currentHeadForward:" << currentHeadForward.x << "\t" << currentHeadForward.y << "\t" << currentHeadForward.z << "\t" <<
			"currentBackForward:" << currentBackForward.x << "\t" << currentBackForward.y << "\t" << currentBackForward.z << "\t" <<
			"currentSwingForward:" << currentSwingForward.x << "\t" << currentSwingForward.y << "\t" << currentSwingForward.z << "\n";*/
	}
}

//六个按键WASDQE
void AirPlane::manualControlAirPlane4() {
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	PxQuat next = body->getGlobalPose().q;
	//直行
	if (turningState2[0]) {
		body->setLinearVelocity(veclocity * currentHeadForward);
	}
	//左偏航
	if (turningState2[1]) {
		PxQuat rot(PxPi / 180 * (0.1*turningSpeed), currentBackForward);
		//PxQuat rot2(PxPi / 180 * (-0.05*turningSpeed), currentHeadForward);
		next = rot * next;
	}
	//右偏航
	if (turningState2[2]) {
		PxQuat rot(PxPi / 180 * (-0.1*turningSpeed), currentBackForward);
		//PxQuat rot2(PxPi / 180 * (0.05*turningSpeed), currentHeadForward);
		next = rot * next;
	}
	//俯冲
	if (turningState2[3]) {
		PxQuat rot(PxPi / 180 * (-0.1*turningSpeed), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	//上仰
	if (turningState2[4]) {
		PxQuat rot(PxPi / 180 * (0.1*turningSpeed), currentSwingForward);
		//next *= rot;
		next = rot * next;
	}
	//左滚转
	if (turningState2[5]) {
		PxQuat rot(PxPi / 180 * (-0.2*turningSpeed), currentHeadForward);
		next = rot * next;
	}
	//右滚转
	if (turningState2[6]) {
		PxQuat rot(PxPi / 180 * (0.2*turningSpeed), currentHeadForward);
		next = rot * next;
	}
	body->setGlobalPose(PxTransform(body->getGlobalPose().p, next));
	currentHeadForward = (body->getGlobalPose().q).rotate(headForward);
	currentBackForward = (body->getGlobalPose().q).rotate(backForward);
	currentSwingForward = (body->getGlobalPose().q).rotate(swingForward);
	PxQuat emitRot(PxPi / 180 * (-10), currentSwingForward);
	emitDirection = emitRot.rotate(currentHeadForward).getNormalized();
	//body->setLinearVelocity(veclocity * currentHeadForward);
}

PxQuat AirPlane::getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront) {
	/*cout << "neededFront:" << neededFront.x << "\t" << neededFront.y << "\t" << neededFront.z << "\t\t"
		<< "bulletFront:" << bulletFront.x << "\t" << bulletFront.y << "\t" << bulletFront.z << "\n";*/
	double cosine = neededFront.getNormalized().dot(bulletFront.getNormalized());
	float angelRadius = acos(cosine);
	PxVec3 rotateAxis = bulletFront.cross(neededFront);
	PxQuat rot(angelRadius, rotateAxis.getNormalized());
	return rot;
}




void AirPlane::emit() {
	//球形弹药
	//PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, body->getGlobalPose().transform(emitTransform), PxSphereGeometry(0.1), *gMaterial, 10.0f);
	//胶囊体弹药
	PxQuat bulletRot(-PxPi / 2, PxVec3(0, 1, 0));
	PxQuat bulletRot2 = body->getGlobalPose().q;
	PxQuat bulletRot3(PxPi / 180 * (-10), currentSwingForward);
	PxVec3 emitPos; PxRigidDynamic* dynamic;
	if (activatemissle) {
		emitPos = body->getGlobalPose().p + (-1)*currentBackForward + (1)*currentHeadForward + leftOrRight * currentSwingForward;
		dynamic = PxCreateDynamic(*gPhysics, PxTransform(emitPos, bulletRot3*bulletRot2*bulletRot),
			PxCapsuleGeometry(0.04, 0.17), *gMaterial, 1.0f);
		dynamic->userData = new UserData(1, "ab", DATATYPE::ACTOR_TYPE::PLANE_MISSLE);
		airPlaneBullet.insert(dynamic);
	}
	else {
		emitPos = body->getGlobalPose().p + (-1)*currentBackForward + (1)*currentHeadForward;
		dynamic = PxCreateDynamic(*gPhysics, PxTransform(emitPos, bulletRot3*bulletRot2*bulletRot),
			PxCapsuleGeometry(0.04, 0.07), *gMaterial, 1.0f);
		dynamic->userData = new UserData(1, "ab", DATATYPE::ACTOR_TYPE::PLANE_BULLET);
	}
	
	leftOrRight *= -1;
	//emitTransform.q = bulletRot;
	//PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, body->getGlobalPose().transform(emitTransform), PxCapsuleGeometry(0.04, 0.07), *gMaterial, 1.0f);
	
	//设置刚体名称
	setupFiltering((PxRigidActor*)(dynamic), FilterGroup::eMISILE, FilterGroup::eMAP|FilterGroup::eTANK);
	dynamic->setName("bullet");
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	dynamic->setLinearVelocity((veclocity + emitVeclocity)*emitDirection);
	//dynamic->setLinearVelocity((veclocity + emitVeclocity)*currentHeadForward);

	
	UserData* temp = reinterpret_cast<UserData*>(dynamic->userData);
	gScene->addActor(*dynamic);
	
}

void AirPlane::reset() {
	body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	PxVec3 p = body->getGlobalPose().p;
	p += 30 * backForward;
	body->setGlobalPose(PxTransform(p, PxQuat(0.0f, backForward)));
	currentHeadForward = headForward;
	currentBackForward = backForward;
	currentSwingForward = swingForward;
	body->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	body->setLinearVelocity(veclocity * currentHeadForward);
}

void AirPlane::getRight(physx::PxVec3& right) { right = currentSwingForward; };

void AirPlane::getFront(physx::PxVec3& front) { front = currentHeadForward; };

void AirPlane::getUp(physx::PxVec3& up) { up = currentBackForward; };

void AirPlane::ProcessKeyPress() {
	//半自动飞行
	/*if (keyToPressState[GLFW_KEY_LEFT]&& turningState[2]) {
		turningState[0] = true;
		turningState[2] = false;
	}
	if (keyToPressState[GLFW_KEY_RIGHT] && turningState[2]) {
		turningState[1] = true;
		turningState[2] = false;
	}
	if (keyToPressState[GLFW_KEY_UP] && turningState[2]) {
		turningState[3] = true;
		turningState[2] = false;
	}
	if (keyToPressState[GLFW_KEY_DOWN] && turningState[2]) {
		turningState[4] = true;
		turningState[2] = false;
	}*/

	//手动飞行
	//上下左右键
	//按下时设true
	//if (keyToPressState[GLFW_KEY_LEFT]) {
	//	turningState[0] = true;
	//}
	//if (keyToPressState[GLFW_KEY_RIGHT]) {
	//	turningState[1] = true;
	//}
	//if (keyToPressState[GLFW_KEY_UP]) {
	//	turningState[3] = true;
	//}
	//if (keyToPressState[GLFW_KEY_DOWN]) {
	//	turningState[4] = true;
	//}
	////松开时设false
	//if (!keyToPressState[GLFW_KEY_LEFT] && keyToPrePressState[GLFW_KEY_LEFT]) {
	//	turningState[0] = false;
	//}
	//if (!keyToPressState[GLFW_KEY_RIGHT] && keyToPrePressState[GLFW_KEY_RIGHT]) {
	//	turningState[1] = false;
	//}
	//if (!keyToPressState[GLFW_KEY_UP] && keyToPrePressState[GLFW_KEY_UP]) {
	//	turningState[3] = false;
	//}
	//if (!keyToPressState[GLFW_KEY_DOWN] && keyToPrePressState[GLFW_KEY_DOWN]) {
	//	turningState[4] = false;
	//}

	//手动控制 W A S D Q E六个按键
	if (keyToPressState[GLFW_KEY_A]) {
		turningState2[1] = true;
	}
	if (keyToPressState[GLFW_KEY_D]) {
		turningState2[2] = true;
	}
	if (keyToPressState[GLFW_KEY_W]) {
		turningState2[3] = true;
	}
	if (keyToPressState[GLFW_KEY_S]) {
		turningState2[4] = true;
	}
	if (keyToPressState[GLFW_KEY_Q]) {
		turningState2[5] = true;
	}
	if (keyToPressState[GLFW_KEY_E]) {
		turningState2[6] = true;
	}
	//松开时设false
	if (!keyToPressState[GLFW_KEY_A] && keyToPrePressState[GLFW_KEY_A]) {
		turningState2[1] = false;
	}
	if (!keyToPressState[GLFW_KEY_D] && keyToPrePressState[GLFW_KEY_D]) {
		turningState2[2] = false;
	}
	if (!keyToPressState[GLFW_KEY_W] && keyToPrePressState[GLFW_KEY_W]) {
		turningState2[3] = false;
	}
	if (!keyToPressState[GLFW_KEY_S] && keyToPrePressState[GLFW_KEY_S]) {
		turningState2[4] = false;
	}
	if (!keyToPressState[GLFW_KEY_Q] && keyToPrePressState[GLFW_KEY_Q]) {
		turningState2[5] = false;
	}
	if (!keyToPressState[GLFW_KEY_E] && keyToPrePressState[GLFW_KEY_E]) {
		turningState2[6] = false;
	}
	if (keyToPressState[GLFW_KEY_1]) {
		activatemissle = false;
	}
	if (keyToPressState[GLFW_KEY_2]) {
		activatemissle = true;
	}
	//发射
	if (!keyToPressState[GLFW_KEY_SPACE] && keyToPrePressState[GLFW_KEY_SPACE]&&((!activatemissle&&bullet_ammo>0)||(activatemissle&&missle_ammo>0))) {
		if (activatemissle) {
			missle_ammo--;
		}
		else {
			bullet_ammo--;
		}
		emit();
		cout<<"bullet_ammo: "<<bullet_ammo<<endl;
	}
	//重置
	if (!keyToPressState[GLFW_KEY_R] && keyToPrePressState[GLFW_KEY_R]) {
		reset();
	}
	//静止/启动
	if (!keyToPressState[GLFW_KEY_C] && keyToPrePressState[GLFW_KEY_C]) {
		if (turningState2[0]) {
			turningState2[0] = !turningState2[0];
			body->setLinearVelocity(PxVec3(0, 0, 0));
		}
		else {
			turningState2[0] = !turningState2[0];
		}
	}
	//加减转向速度
	if (!keyToPressState[GLFW_KEY_Z] && keyToPrePressState[GLFW_KEY_Z]) {
		//-
		turningSpeed = max(turningSpeed - 1, 1);
	}
	if (!keyToPressState[GLFW_KEY_X] && keyToPrePressState[GLFW_KEY_X]) {
		//-
		turningSpeed = min(turningSpeed + 1, 5);
	}
	//打印飞机globalPosition
	if (!keyToPressState[GLFW_KEY_P] && keyToPrePressState[GLFW_KEY_P]) {
		cout << "位置：" << this->body->getGlobalPose().p.x << "\t" 
			<< this->body->getGlobalPose().p.y << "\t" << this->body->getGlobalPose().p.z << "\n";
	}
};
void AirPlane::oncontact(DATATYPE::ACTOR_TYPE _type) {
	int damage = int(_type) * 2;
	if (this->health - damage > 0) {
		this->health -= damage;
		cout << "Plane - " << damage << endl;
	}
	else if(this->alive==true) {
		this->health = 0;
		this->alive = false;
		cout << "Plane died" << endl;
	}
}
void AirPlane::oncontact(DATATYPE::TRIGGER_TYPE _type) {
	if (_type == DATATYPE::TRIGGER_TYPE::SUPPLY ) {
		this->bullet_ammo += 15;
		if (this->health + 20 >= 100) {
			this->health = 100;
		}
		else {
			this->health += 20;
		}
		cout << bullet_ammo<<'\t'<<health << endl;
	}
	else if(_type == DATATYPE::TRIGGER_TYPE::COLLECTION){
		this->missle_ammo += 5;
		cout << "missle_ammo"<<missle_ammo << endl;
	}
	else {}
}
void AirPlane::formcloud() {
	vector<string>textures;
	
	//textures.push_back("images/textures/smoke/smoke-white-1.png");
	textures.push_back("images/textures/smoke/smoke-gray-0.png");
	PxVec3 pos1 = body->getGlobalPose().p + (-1)*currentHeadForward+ 2*currentSwingForward;
	CloudParticleCluster* cloud_cluster1 = new CloudParticleCluster(
			70, 0.05f,  //云密度、云团的半径
			0.05f, 3.4f, // 云在y方向的速度、云在y方向上最大能上升的距离
			glm::vec3(pos1.x, pos1.y, pos1.z), //初始位置
			glm::vec3(0.03f, 0.03f, 0.03f), //每片云粒子的缩放
			//camera.getPosition() + camera.getFront() * 1.f,
			textures, // 纹理路径列表
			cloudShader //渲染此烟雾的shader
		);
	PxVec3 pos2 = body->getGlobalPose().p + (-1)*currentHeadForward +(-2)*currentSwingForward;
	CloudParticleCluster* cloud_cluster2 = new CloudParticleCluster(
		70, 0.05f,  //云密度、云团的半径
		0.05f, 3.4f, // 云在y方向的速度、云在y方向上最大能上升的距离
		glm::vec3(pos2.x, pos2.y, pos2.z), //初始位置
		glm::vec3(0.03f, 0.03f, 0.03f), //每片云粒子的缩放
		//camera.getPosition() + camera.getFront() * 1.f,
		textures, // 纹理路径列表
		cloudShader //渲染此烟雾的shader
	);
	//textures.clear();
	//textures.push_back("images/textures/smoke/smoke-gray-0.png");
	PxVec3 pos3 = body->getGlobalPose().p + (-1)*currentHeadForward ;
	CloudParticleCluster* cloud_cluster3 = new CloudParticleCluster(
		10, 0.005f,  //云密度、云团的半径
		0.05f, 3.4f, // 云在y方向的速度、云在y方向上最大能上升的距离
		glm::vec3(pos3.x, pos3.y, pos3.z), //初始位置
		glm::vec3(0.1f, 0.1f, 0.1f), //每片云粒子的缩放
		//camera.getPosition() + camera.getFront() * 1.f,
		textures, // 纹理路径列表
		cloudShader //渲染此烟雾的shader
	);
	renderParticleClusterList.push_back(cloud_cluster1);
	renderParticleClusterList.push_back(cloud_cluster2);
	renderParticleClusterList.push_back(cloud_cluster3);
}
void AirPlane::formmisslecloud() {
	for (auto i = airPlaneBullet.begin(); i != airPlaneBullet.end(); i++) {
		PxVec3 pos = (*i)->getGlobalPose().p ;
		vector<string>textures;
		//textures.push_back("images/textures/smoke/smoke-white-1.png");
		textures.push_back("images/textures/smoke/smoke-gray-0.png");
		CloudParticleCluster* cloud_cluster = new CloudParticleCluster(
			10, 0.05f,  //云密度、云团的半径
			0.1f, 4.0f, // 云在y方向的速度、云在y方向上最大能上升的距离
			glm::vec3(pos.x, pos.y, pos.z), //初始位置
			glm::vec3(0.1f, 0.1f, 0.1f), //每片云粒子的缩放
			//camera.getPosition() + camera.getFront() * 1.f,
			textures, // 纹理路径列表
			cloudShader //渲染此烟雾的shader
		);
		renderParticleClusterList.push_back(cloud_cluster);
	}
}










Player::Player(physx::PxRigidDynamic* target,AirPlane* airplane) :BaseCharacter(target) {
	this->body = target;
	this->airPlane = airplane;
	cout << "飞机速度：" << this->airPlane->getRigid()->getLinearVelocity().x << "\t"
		<< this->airPlane->getRigid()->getLinearVelocity().y << "\t" << this->airPlane->getRigid()->getLinearVelocity().z << "\n";
	this->rigid->setName("vehicle");
	this->rigid->setAngularDamping(0.5f);
	this->rigid->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
	this->born = target->getGlobalPose().p;
	this->rigid->userData = new UserData(this, 1, "tank", DATATYPE::ACTOR_TYPE::TANK);
	currentheadforward = headforward;
	currentbackforward = backforward;
	autoshooting = true;
	turnningState.resize(2);
	turnningState[0] = true;
	//绑定血条shape
	PxTransform pos(this->body->getGlobalPose().p + PxVec3(0, 5, 0));
	this->healthBody = createAndShowBlood(this->body, this->healthLength, pos, PxTransform(PxVec3(0, 4, 0)), PxTransform(PxVec3(0, -1, 0)));

	gScene->addActor(*(this->rigid));
	gScene->addActor(*(this->healthBody));
}

void Player::ProcessKeyPress() {
	if (this->rigid == nullptr)return;
	//physx::PxVec3 totalvelocity(0.0f, 0.0f, 0.0f);
	//physx::PxVec3 prevelocity = this->rigid->getLinearVelocity();
	PxQuat next = this->rigid->getGlobalPose().q;
	float current_velocity = 0;
	if (keyToPressState[GLFW_KEY_W]) {
		if (current_velocity < 35.0f) {
			current_velocity += 2.0f;
		}
	}
	if (keyToPressState[GLFW_KEY_S]) {
		if (current_velocity > -15.0f) {
			current_velocity -= 1.6f;
		}
	}
	if (keyToPressState[GLFW_KEY_A]) {
		PxQuat rot(PxPi / 180 * (0.2), currentbackforward);
		//PxQuat rot2(PxPi / 180 * (-0.1), currentheadforward);
		next = rot * next;
	}
	if (keyToPressState[GLFW_KEY_D]) {
		PxQuat rot(PxPi / 180 * (-0.2), currentbackforward);
		//PxQuat rot2(PxPi / 180 * (0.1), currentheadforward);
		next = rot * next;
	}
	this->rigid->setGlobalPose(PxTransform(this->rigid->getGlobalPose().p, next));
	currentheadforward = (this->rigid->getGlobalPose().q).rotate(headforward);
	currentbackforward = (this->rigid->getGlobalPose().q).rotate(backforward);
	this->rigid->setLinearVelocity(current_velocity * currentheadforward);

	if (keyToPressState[GLFW_KEY_F]) {
		PxTransform px;
		cout << "F" << endl;
		PxVec3 mDir; glmVec3ToPxVec3(camera.getFront(), mDir);
		PxVec3 mEye; glmVec3ToPxVec3(camera.getPosition(), mEye);
		PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0));

		if (viewY.normalize() < 1e-6f)
			px = PxTransform(mEye);
		else {
			PxMat33 m(mDir.cross(viewY), viewY, -mDir);
			px = PxTransform(mEye, PxQuat(m));
		}
		px.p = this->rigid->getGlobalPose().p + glmVec3ToPxVec3(camera.getFront() * 2.f) + PxVec3(0.0f, 4.0f, 3.0f);
		if (autoshooting) {
			//createshell(px, px.rotate(PxVec3(0, 0, -1)) * 200);
			fire(px, px.rotate(PxVec3(0, 0, -1)) * 200);
		}
		else {
			clock_t now = clock();
			if (now - last > 1000) {
				//createshell(px, px.rotate(PxVec3(0, 0, -1)) * 200);
				fire(px, px.rotate(PxVec3(0, 0, -1)) * 200);
				last = now;
			}
		}
	}
	if (keyToPrePressState[GLFW_KEY_H] && !keyToPressState[GLFW_KEY_H]) {
		trackangle(this->rigid->getGlobalPose().p, PxVec3(15.0f, 5.0f, -20.0f));
	}
	if (keyToPrePressState[GLFW_KEY_I] && !keyToPressState[GLFW_KEY_I]) {
		forward(PxVec3(15.0f, 5.0f, -20.0f), 3.0f);
	}
	if (keyToPrePressState[GLFW_KEY_T] && !keyToPressState[GLFW_KEY_T]) {
		autoshooting = !autoshooting;
	}

	if (keyToPrePressState['`']) {
		this->rigid->setGlobalPose(PxTransform(born));
	}
}
PxQuat Player::getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront) {
	/*cout << "neededFront:" << needfront.x << "\t" << needfront.y << "\t" << needfront.z << "\t\t"
		<< "bulletFront:" << bulletfront.x << "\t" << bulletfront.y << "\t" << bulletfront.z << "\n";*/
	double cosine = needfront.getNormalized().dot(bulletfront.getNormalized());
	float angelRadius = acos(cosine);
	PxVec3 rotateAxis = bulletfront.cross(needfront);
	PxQuat rot(angelRadius, rotateAxis.getNormalized());
	return rot;
}
void Player::fire(const PxTransform& t, const PxVec3& velocity) {
	PxQuat bulletRot(-PxPi / 2, PxVec3(0, 1, 0));
	/*emitTransform.q = getBulletRotate(t, PxVec3(1.0f, 0.0f, 0.0f));*/
	//PxTransform emitTransform = PxTransform(currentbackforward * 1);
	PxTransform emitTransform = t;
	/*emitTransform.q = bulletRot;*/
	emitTransform.q = getshellrotate(velocity, PxVec3(1.0f, 0.0f, 0.0f));
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, emitTransform, PxCapsuleGeometry(0.04, 0.15), *gMaterial, 1000.0f);
	//设置刚体名称
	dynamic->setName("bullet");
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	dynamic->setLinearVelocity(velocity);
	/*UserData data;
	(data).id = 1;
	(data).name = "littleBall";
	(data).health = 10;
	cout << data.id << endl;*/

	dynamic->userData = new UserData(1, "ab",DATATYPE::ACTOR_TYPE::TANK_BULLET);
	UserData* temp = reinterpret_cast<UserData*>(dynamic->userData);
	//cout << temp->id << endl;
	//cout << a << endl;
	gScene->addActor(*dynamic);
}
int Player::trackangle(PxVec3& start, PxVec3& destination) {
	PxVec3 dir = destination - start;
	//cout << dir.x << " " << dir.y << " " << dir.z << endl;
	PxQuat next = this->rigid->getGlobalPose().q;
	double angle = acos(dir.getNormalized().dot(currentheadforward.getNormalized()));
	PxVec3 axis = currentheadforward.cross(dir);
	//double angle = acos(dir.dot(this->currentheadforward) / ((dir.normalize())*(this->currentheadforward.normalize())));
	//cout << angle << endl;
	if (angle < 0.1)return 1;
	//PxQuat rot(angle, currentbackforward);
	PxQuat rot(angle, axis.getNormalized());
	next = rot * next;
	this->rigid->setGlobalPose(PxTransform(this->rigid->getGlobalPose().p, next));
	currentheadforward = (this->rigid->getGlobalPose().q).rotate(headforward);
	//cout << currentheadforward.x << " " << currentheadforward.y<< " " << currentheadforward.z << endl;
	currentbackforward = (this->rigid->getGlobalPose().q).rotate(backforward);
	return 0;
}
int Player::forward(PxVec3& dir, double velocity) {
	PxVec3 current_pos = this->rigid->getGlobalPose().p;
	//cout << currentheadforward.x << " " << currentheadforward.y << " " << currentheadforward.z << endl;
	double distance = sqrt(pow(current_pos.x - dir.x, 2) + pow(current_pos.y - dir.y, 2) + pow(current_pos.z - dir.z, 2));
	//cout << distance << endl;
	this->rigid->setLinearVelocity((this->currentheadforward)*velocity);
	return 0;
}
void Player::automove() {
	if (this->alive == false)return;
	fireTime++;
	
	if (fireTime % 200 == 0) {
		autoEmit();
	}
	if (turnningState[0]) {
		this->rigid->setLinearVelocity(currentheadforward*this->velocity);
		if (this->rigid->getGlobalPose().p.z >= 56&& this->rigid->getGlobalPose().p.z+20*currentheadforward.z>=56) {
			turnningState[0] = false;
			turnningState[1] = true;
		}
		else if (this->rigid->getGlobalPose().p.z <= 28 && this->rigid->getGlobalPose().p.z + 20 * currentheadforward.z <= 28) {
			turnningState[0] = false;
			turnningState[1] = true;
		}
	}
	else {
		currentAngle += 1;
		PxQuat rot(PxPi / 180 * (1), currentbackforward);
		this->rigid->setGlobalPose(PxTransform(this->rigid->getGlobalPose().p, rot*this->rigid->getGlobalPose().q));
		currentheadforward = (rot*this->rigid->getGlobalPose().q).rotate(headforward);
		currentbackforward = (rot*this->rigid->getGlobalPose().q).rotate(backforward);
		if (currentAngle % 180 == 0) {
			PxQuat rot(PxPi * (currentAngle/180), currentbackforward);
			this->rigid->setGlobalPose(PxTransform(this->rigid->getGlobalPose().p, rot));
			currentheadforward = (rot).rotate(headforward);
			currentbackforward = (rot).rotate(backforward);
			if (currentAngle % 360 == 0)currentAngle = 0;
			turnningState[0] = true;
			turnningState[1] = false;
		}
	}
}
void Player::autoEmit() {
	//空间中向量
	PxVec3 airPlaneVelocity = this->airPlane->getRigid()->getLinearVelocity();
	PxVec3 airPlanePos = this->airPlane->getGlobalPose();
	PxVec3 tankPos = this->rigid->getGlobalPose().p;	tankPos.y += 5;//炮口往上偏
	PxVec3 tankToPlane = airPlanePos - tankPos;

	//飞机不动则直接发射弹药
	if (airPlaneVelocity.magnitude() <= 1e-5) {
		PxVec3 rotAxis2 = (PxVec3(1, 0, 0).cross(tankToPlane)).getNormalized();
		float rotCos = PxVec3(1, 0, 0).dot(tankToPlane.getNormalized());
		float rotAngle2 = acos(rotCos);
		PxQuat rot2(PxPi / 2, PxVec3(0, 0, 1));
		PxQuat rot3(rotAngle2, rotAxis2);

		PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, PxTransform(tankPos, rot3), PxCapsuleGeometry(0.04, 0.15), *gMaterial, 0.1f);
		//设置刚体名称
		dynamic->setName("bullet");
		setupFiltering(dynamic, FilterGroup::ePLAYERBULLET, FilterGroup::eAIRPLANE);
		gScene->addActor(*dynamic);
		dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
		dynamic->setLinearVelocity(bulletVelocity*tankToPlane.getNormalized());
		return;
	}

	//部分向量的大小
	double distance = (tankToPlane).magnitude();
	double air_plane_v = airPlaneVelocity.magnitude();
	double tank_bullet_v = this->bulletVelocity;
	double cos_airPlaneVelocity_tankToPlane = (airPlaneVelocity.getNormalized()).dot(-tankToPlane.getNormalized());
	//求解时间t
	double a = pow(air_plane_v, 2) - pow(tank_bullet_v, 2);
	double b = (-2)*cos_airPlaneVelocity_tankToPlane*air_plane_v*distance;
	double c = distance * distance;
	double delta = b * b - 4 * a*c;
	if (delta < 0)return;
	double t1 = (-b - sqrt(delta)) / (2 * a);
	double t2 = (-b + sqrt(delta)) / (2 * a);
	if (t1 > t2) {
		double temp = t1;
		t1 = t2;
		t2 = temp;
	}
	if (t2 < 0)return;
	//取较小的正值
	double t = t1 > 0 ? t1 : t2;
	//求解炮弹相对于飞机的发射角度
	double aa = tank_bullet_v * t;
	double bb = distance;
	double cc = air_plane_v * t;
	double cos_xita = (aa*aa + bb * bb - cc * cc) / (2 * aa*bb);
	//获得需旋转的弧度值
	double rotateAngle = acos(cos_xita);
	PxVec3 rotateAxis = ((tankToPlane).cross(airPlaneVelocity)).getNormalized();
	PxQuat rot(rotateAngle, rotateAxis);
	//坦克子弹最终出射方向
	PxVec3 emitDirection = (rot.rotate(tankToPlane)).getNormalized();
	//fire(PxTransform(tankPos), emitDirection);
	PxVec3 rotAxis2 = (PxVec3(1, 0, 0).cross(emitDirection)).getNormalized();
	float rotCos = PxVec3(1, 0, 0).dot(emitDirection.getNormalized());
	float rotAngle2 = acos(rotCos);
	PxQuat rot2(PxPi/2, PxVec3(0, 0, 1));
	PxQuat rot3(rotAngle2, rotAxis2);
	
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, PxTransform(tankPos, rot3), PxCapsuleGeometry(0.04, 0.15), *gMaterial, 0.1f);
	//设置刚体名称
	dynamic->setName("bullet");
	setupFiltering(dynamic, FilterGroup::ePLAYERBULLET, FilterGroup::eAIRPLANE);
	gScene->addActor(*dynamic);
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	dynamic->setLinearVelocity(bulletVelocity*emitDirection);
	dynamic->userData = new UserData(1, "ab",DATATYPE::ACTOR_TYPE::TANK_BULLET);
	setupFiltering(dynamic, FilterGroup::eTowerBullet, FilterGroup::ePlayer);
}
void Player::oncontact(DATATYPE::ACTOR_TYPE _type) {
	int damage = int(_type) * 2;
	if (this->health - damage > 0) {
		this->health -= damage;
		cout << "Tank - " << damage << endl;
		updateTankList.insert(this);
	}
	else if(this->alive==true) {
		this->health = 0;
		updateTankList.insert(this);
		this->alive = false;
		Logger::debug(this->getGlobalPose());
		bonus::generate_bonus_pos(this->rigid->getGlobalPose());
		cout << "Tank died" << endl;
	}
}