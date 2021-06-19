#include "Controller.h"



//全局变量区
const float velocity = 0.5f;
extern Camera camera;


bool autoshooting = true;//射击机制
clock_t last = 0;

void keyPress() {

	if (!camera.isHandling()) {
		vehicleProcessKeyboard();
		playerProcessKeyboard();
	}

}





void mouseMove() {

}



void vehicleProcessKeyboard() {
	if (player == nullptr) return;
	PxVec3 prevelocity = player->getLinearVelocity();
	if (keyToPressState[GLFW_KEY_W]) {
		PxVec3 totalvelocity = prevelocity + glmVec3ToPxVec3(camera.getFront() * velocity);
		player->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_S]) {
		PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getFront() * velocity);
		player->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_A]) {
		PxVec3 totalvelocity = prevelocity - glmVec3ToPxVec3(camera.getRight() * velocity);
		player->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_D]) {
		PxVec3 totalvelocity = prevelocity + glmVec3ToPxVec3(camera.getRight() * velocity);;
		player->setLinearVelocity(totalvelocity);
	}
	if (keyToPrePressState[GLFW_KEY_SPACE] && !keyToPressState[GLFW_KEY_SPACE]) {
		PxVec3 jumpup(0.0f, 500, 0.0f);
		player->addForce(jumpup);
	}

	//if (keyToPressState[GLFW_KEY_U]) {
	//	PxVec3 forward_velocity(0.0f, 0.0f, (-1)*velocity);
	//	player->setLinearVelocity(forward_velocity);
	//}
	//	vehicle.ProcessKeyboard(Player_FORWARD, deltaTime);
	//if (keyToPressState[GLFW_KEY_J])
	//	vehicle.ProcessKeyboard(Player_BACKWARD, deltaTime);
	//if (keyToPressState[GLFW_KEY_H])
	//	vehicle.ProcessKeyboard(Player_LEFT, deltaTime);
	//if (keyToPressState[GLFW_KEY_K])
	//	vehicle.ProcessKeyboard(Player_RIGHT, deltaTime);


}

void playerProcessKeyboard() {
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

	if (keyToPressState[GLFW_KEY_F]) {
		if (autoshooting) {
			createDynamic(px, PxSphereGeometry(0.1f), px.rotate(PxVec3(0, 0, -1)) * 20);
		}
		else {
			clock_t now = clock();
			if (now - last > 1000) {
				createDynamic(px, PxSphereGeometry(0.1f), px.rotate(PxVec3(0, 0, -1)) * 20);
				last = now;
			}
		}
	}

	if (keyToPrePressState[GLFW_KEY_T] && !keyToPressState[GLFW_KEY_T]) {
		autoshooting = !autoshooting;
	}

	/*if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		keyPress('F', px);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {//切换连发
		keyboard_input[GLFW_KEY_T] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) && (keyboard_input[GLFW_KEY_T] == true)) {
		keyboard_input[GLFW_KEY_T] = false;
		keyPress('T', px);
	}*/
}