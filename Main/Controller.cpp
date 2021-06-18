#include "Controller.h"



//全局变量区
const float velocity = 0.5f;
PxVec3 forward_velocity(0.0f, 0.0f, (-1)*velocity);
PxVec3 backward_velocity(0.0f, 0.0f, velocity);
PxVec3 leftward_velocity((-1)*velocity, 0.0f, 0.0f);
PxVec3 rightward_velocity(velocity, 0.0f, 0.0f);

bool autoshooting = true;//射击机制
clock_t last = 0;

void keypress() {

	vehicleProcessKeyboard();
	playerProcessKeyboard();
}





void mouseMove() {

}



void vehicleProcessKeyboard() {
	PxVec3 prevelocity = player_ctl->getLinearVelocity();
	if (keyToPressState[GLFW_KEY_U]) {
		PxVec3 totalvelocity = prevelocity + forward_velocity;
		player_ctl->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_J]) {
		PxVec3 totalvelocity = prevelocity + backward_velocity;
		player_ctl->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_H]) {
		PxVec3 totalvelocity = prevelocity + leftward_velocity;
		player_ctl->setLinearVelocity(totalvelocity);
	}
	if (keyToPressState[GLFW_KEY_K]) {
		PxVec3 totalvelocity = prevelocity + rightward_velocity;
		player_ctl->setLinearVelocity(totalvelocity);
	}
	if (keyToPrePressState[GLFW_KEY_SPACE] && !keyToPressState[GLFW_KEY_SPACE]) {
		PxVec3 jumpup(0.0f, 1000.0f, 0.0f);
		player_ctl->addForce(jumpup);
	}

	//if (keyToPressState[GLFW_KEY_U]) {
	//	PxVec3 forward_velocity(0.0f, 0.0f, (-1)*velocity);
	//	player_ctl->setLinearVelocity(forward_velocity);
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

	PxVec3 mDir; glmVec3ToPxVec3(camera.Front, mDir);
	PxVec3 mEye; glmVec3ToPxVec3(camera.Position, mEye);
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
	
	if (keyToPrePressState[GLFW_KEY_T]&&!keyToPressState[GLFW_KEY_T]) {
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