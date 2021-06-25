#include "Controller.h"


//全局变量区
const float velocity = 1.0f;
extern Camera camera;

extern vector<bool>	turningState;
extern AirPlane* Plane_1;
extern PxRigidDynamic* player;
extern Player* vehicle;
extern PxTransform born_pos;

extern Shader* envShader;
extern Shader* smokeParticleShader;

bool autoshooting = true;//射击机制
clock_t last = 0;


void processOtherControlEvents() {
	mouseClick();
}

void keyPress() {

	//if (!camera.isHandling()) {
		//vehicleProcessKeyboard();
		//playerProcessKeyboard();
	vehicle->ProcessKeyPress();
	Plane_1->ProcessKeyPress();
	//mouseClick();
//}

}


void mouseMove() {

}

void mouseClick() {
	if (mouseButtonPressState[GLFW_MOUSE_BUTTON_LEFT]) {
		PxTransform px;

		PxVec3 mDir; glmVec3ToPxVec3(camera.getFront(), mDir);
		PxVec3 mEye; glmVec3ToPxVec3(camera.getPosition(), mEye);
		PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0));

		if (viewY.normalize() < 1e-6f)
			px = PxTransform(mEye);
		else {
			PxMat33 m(mDir.cross(viewY), viewY, -mDir);
			px = PxTransform(mEye, PxQuat(m));
		}
		if (camera.getMode() == VIEW_TYPE::THIRD_PERSON) {
			px.p = player->getGlobalPose().p + glmVec3ToPxVec3(camera.getFront() * 2.f);
			createBullet(px, px.rotate(PxVec3(0, 0, -1)) * 200);
		}

	}
	if (mouseButtonPressState[GLFW_MOUSE_BUTTON_RIGHT]) { //鼠标右键
		createParticles(
			1000, false,
			new SmokeParticle(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), smokeParticleShader),
			glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f),
			false, 2.0, // true是散开
			true, 20.0, // true是随机速度
			20, 5,
			PxVec3(0.f, 0.f, 0.f), //初始速度
			PxVec3(0.f, 0.f, 0.f)  //力场
		);

		//createParticles(
		//	100, false,
		//	//new SmokeParticle(glm::vec3(0.f, 0.f, 0.f), glm::vec3(50.f, 50.f, 50.f), glm::vec3(1.f, 1.f, 0.f), smokeParticleShader),
		//	new Cube(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.01f, 0.01f, 0.01f), "", envShader, "images/textures/smoke.png"),
		//	glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f),
		//	true, 0.5,
		//	false, 20.0,
		//	20, 5,
		//	PxVec3(0.f, 0.f, 0.f),
		//	PxVec3(0.f, 10.f, 0.f)
		//);

	}
}


//void vehicleProcessKeyboard() {
//	if (vehicle == nullptr) return;
//	/*PxVec3 forward(0.0f,0.0f,0.0f);
//	PxVec3 backward(0.0f, 0.0f, 0.0f);
//	PxVec3 rightward(0.0f, 0.0f, 0.0f);
//	PxVec3 leftward(0.0f, 0.0f, 0.0f);*/
//	PxVec3 totalvelocity(0.0f, 0.0f, 0.0f);
//	PxVec3 prevelocity = vehicle->getLinearVelocity();
//
//	//cout << "x: " << prevelocity.x << "y: " << prevelocity.y << "z: " << prevelocity.z << endl;
//	if (keyToPressState[GLFW_KEY_W]) {
//		//glmVec3ToPxVec3(camera.getFront() * velocity,forward);
//		totalvelocity += glmVec3ToPxVec3(camera.getFront() * velocity);
//		//player->setLinearVelocity(totalvelocity);
//	}
//	if (keyToPressState[GLFW_KEY_S]) {
//		//PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getFront() * velocity);
//		//PxVec3 totalvelocity = PxVec3(prevelocity.x, prevelocity.y,(-1)*velocity);
//		//player->setLinearVelocity(totalvelocity);
//		//glmVec3ToPxVec3(-(camera.getFront() * velocity), backward);
//		totalvelocity += -glmVec3ToPxVec3(camera.getFront() * velocity);
//	}
//	if (keyToPressState[GLFW_KEY_A]) {
//		//PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getRight() * velocity);
//		//player->setLinearVelocity(totalvelocity);
//		//glmVec3ToPxVec3(-(camera.getRight() * velocity), leftward);
//		totalvelocity += -glmVec3ToPxVec3(camera.getRight() * velocity);
//	}
//	if (keyToPressState[GLFW_KEY_D]) {
//		//PxVec3 totalvelocity = prevelocity + glmVec3ToPxVec3(camera.getRight() * velocity);;
//		//player->setLinearVelocity(totalvelocity);
//		//glmVec3ToPxVec3((camera.getRight() * velocity), rightward);
//		totalvelocity += glmVec3ToPxVec3(camera.getRight() * velocity);
//	}
//
//	/*if (keyToPrePressState[GLFW_KEY_SPACE] && !keyToPressState[GLFW_KEY_SPACE]) {
//		PxVec3 jumpup(0.0f, 500, 0.0f);
//		player->addForce(jumpup);
//	}*/
//	vehicle->setLinearVelocity(totalvelocity + prevelocity);
//	if (keyToPrePressState['`']) {
//		player->setGlobalPose(born_pos);
//	}
//	//player->setLinearVelocity(forward + backward + leftward + rightward);
//	//if (keyToPressState[GLFW_KEY_U]) {
//	//	PxVec3 forward_velocity(0.0f, 0.0f, (-1)*velocity);
//	//	player->setLinearVelocity(forward_velocity);
//	//}
//	//	vehicle.ProcessKeyboard(Player_FORWARD, deltaTime);
//	//if (keyToPressState[GLFW_KEY_J])
//	//	vehicle.ProcessKeyboard(Player_BACKWARD, deltaTime);
//	//if (keyToPressState[GLFW_KEY_H])
//	//	vehicle.ProcessKeyboard(Player_LEFT, deltaTime);
//	//if (keyToPressState[GLFW_KEY_K])
//	//	vehicle.ProcessKeyboard(Player_RIGHT, deltaTime);
//}

//void playerProcessKeyboard() {
//	PxTransform px;
//	
//	if (player == nullptr) return;
//	PxVec3 forward(0.0f, 0.0f, 0.0f);
//	PxVec3 backward(0.0f, 0.0f, 0.0f);
//	PxVec3 rightward(0.0f, 0.0f, 0.0f);
//	PxVec3 leftward(0.0f, 0.0f, 0.0f);
//
//	PxVec3 mDir; glmVec3ToPxVec3(camera.getFront(), mDir);
//	PxVec3 mEye; glmVec3ToPxVec3(camera.getPosition(), mEye);
//	PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0));
//
//	if (viewY.normalize() < 1e-6f)
//		px = PxTransform(mEye);
//	else {
//		PxMat33 m(mDir.cross(viewY), viewY, -mDir);
//		px = PxTransform(mEye, PxQuat(m));
//	}
//
//	if (keyToPressState[GLFW_KEY_F]) {
//		if (autoshooting) {
//			createDynamic(px, PxSphereGeometry(0.1f), px.rotate(PxVec3(0, 0, -1)) * 20);
//		}
//		else {
//			clock_t now = clock();
//			if (now - last > 1000) {
//				createDynamic(px, PxSphereGeometry(0.1f), px.rotate(PxVec3(0, 0, -1)) * 20);
//				last = now;
//			}
//		}
//	}
//
//	if (keyToPrePressState[GLFW_KEY_T] && !keyToPressState[GLFW_KEY_T]) {
//		autoshooting = !autoshooting;
//	}
//	if (keyToPressState[GLFW_KEY_W]) {
//		glmVec3ToPxVec3(camera.getFront() * velocity, forward);
//		//PxVec3 totalvelocity = prevelocity + glmVec3ToPxVec3(camera.getFront() * velocity);
//		//player->setLinearVelocity(totalvelocity);
//	}
//	if (keyToPressState[GLFW_KEY_S]) {
//		//PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getFront() * velocity);
//		//PxVec3 totalvelocity = PxVec3(prevelocity.x, prevelocity.y,(-1)*velocity);
//		//player->setLinearVelocity(totalvelocity);
//		glmVec3ToPxVec3(-(camera.getFront() * velocity), backward);
//	}
//	if (keyToPressState[GLFW_KEY_A]) {
//		//PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getRight() * velocity);
//		//player->setLinearVelocity(totalvelocity);
//		glmVec3ToPxVec3(-(camera.getRight() * velocity), leftward);
//	}
//	if (keyToPressState[GLFW_KEY_D]) {
//		//PxVec3 totalvelocity = prevelocity + glmVec3ToPxVec3(camera.getRight() * velocity);;
//		//player->setLinearVelocity(totalvelocity);
//		glmVec3ToPxVec3((camera.getRight() * velocity), rightward);
//	}
//
//	if (keyToPrePressState[GLFW_KEY_SPACE] && !keyToPressState[GLFW_KEY_SPACE]) {
//		PxVec3 jumpup(0.0f, 5000, 0.0f);
//		player->addForce(jumpup);
//	}
//	if (keyToPressState['`']) {
//		player->setGlobalPose(born_pos);
//	}
//	player->setLinearVelocity(forward + backward + leftward + rightward);
//}
