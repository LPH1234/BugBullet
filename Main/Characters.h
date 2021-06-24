#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include "GLFW/glfw3.h"
#include "PxRigidDynamic.h"
#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#include "../Utils/Convert.h"


class BaseCharacter {
protected:
	physx::PxRigidDynamic* rigid = nullptr;
public:
	BaseCharacter(physx::PxRigidDynamic* target) {
		this->rigid = target;
	}

	BaseCharacter(physx::PxRigidDynamic* target, physx::PxTransform& t) {
		this->rigid = target;
		this->rigid->setGlobalPose(t);
	}

	virtual physx::PxVec3 getGlobalPose() {
		return rigid->getGlobalPose().p;
	}

	virtual void getRight(physx::PxVec3& right) {};
	virtual void getFront(physx::PxVec3& front) {};
	virtual void getUp(physx::PxVec3& up) {};
	virtual void ProcessKeyPress() {};
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick() {};

	physx::PxRigidDynamic* getRigid() {
		return this->rigid;
	}

	void setRotate(physx::PxQuat& q) {
		this->rigid->setGlobalPose(physx::PxTransform(this->rigid->getGlobalPose().p, q));
	}

	void rotateBy(physx::PxQuat& q) {
		this->rigid->setGlobalPose(physx::PxTransform(this->rigid->getGlobalPose().p, this->rigid->getGlobalPose().q * q));
	}

	void setGloPosition(physx::PxVec3& p) {
		this->rigid->setGlobalPose(physx::PxTransform(p, this->rigid->getGlobalPose().q));
	}

	void moveBy(physx::PxVec3& p) {
		this->rigid->setGlobalPose(physx::PxTransform(this->rigid->getGlobalPose().p + p, this->rigid->getGlobalPose().q));
	}

	void setTransform(physx::PxTransform& t) {
		this->rigid->setGlobalPose(t);
	}
};


class Player : public BaseCharacter {
private:
	PxVec3 headforward = PxVec3(1.0f, 0.0f, 0.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
public:
	Player(physx::PxRigidDynamic* target) :BaseCharacter(target) {};
	void ProcessKeyPress();
};
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
//physx::PxRigidDynamic* initvehicle(const PxTransform& t, const PxGeometry& geometry) {
//	if (!t.isValid()) {
//		Logger::error("error:");
//	}
//	PxMaterial* me = gPhysics->createMaterial(0.0f, 0.8f, 0.0f);
//	//player = PxCreateDynamic(*gPhysics, t, geometry, *me, 1.0f);
//	vehicle = reinterpret_cast<PxRigidDynamic*>(createModel(glm::vec3(10.0f, 50.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "model/vehicle/82-koenigsegg-agera/a.obj", envShader, false));
//
//	PxVec3 position = vehicle->getGlobalPose().p;
//	//cout << "position: " << "x: " << position.x << " y: " << position.y << " z: " << position.z << endl;
//	cout << "create vehicle" << endl;
//	//设置刚体名称
//	vehicle->setName("vehicle");
//
//	//userdata指向自己
//	//dynamic->userData = dynamic;
//	//设置碰撞的标签
//	setupFiltering(vehicle, FilterGroup::eBALL, FilterGroup::eSTACK);
//	me->release();
//
//	vehicle->setAngularDamping(0.5f);
//	vehicle->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
//	//dynamic->setLinearVelocity(velocity);
//	gScene->addActor(*vehicle);
//	return vehicle;
//}
class Plane : public BaseCharacter {



};