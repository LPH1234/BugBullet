#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include "GLFW/glfw3.h"
#include "PxRigidDynamic.h"
#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#include "../Utils/Convert.h"
#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include "../Render/objLoader.h"
#include "../Utils/module.h"
#include "../Render/models.h"
#include "../Render/Render.h"
#include "../Data/Data.h"
#include <cmath>
#include "../Data/Consts.h"


extern std::unordered_map<int, bool> keyToPressState;
extern std::unordered_map<int, bool> keyToPrePressState;
extern bool mouseButtonPressState[3];
extern PxPhysics*				gPhysics;
extern PxScene*					gScene;
extern PxMaterial*				gMaterial;
extern Camera camera;
extern PxTransform born_pos;
extern const float velocity;

//extern void createshell(const PxTransform& t, const PxVec3& velocity);

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
	PxVec3 headforward = PxVec3(0.0f, 0.0f, -1.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 currentheadforward;
	PxVec3 currentbackforward;
	bool autoshooting;//射击机制
	clock_t last = 0;
	PxVec3 born;
	vector<PxVec3> waypoint;
public:
	PxRigidDynamic*			body;//刚体
	Player(physx::PxRigidDynamic* target) ;
	void ProcessKeyPress();
	void fire(const PxTransform& t,const PxVec3& velocity);
	PxQuat getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront);
	int trackangle(PxVec3& start,PxVec3& destination);
	int forward(PxVec3& dir, double velocity);
	void automove();
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


class AirPlane : public BaseCharacter {
public:
	PxRigidDynamic*			body;//飞机刚体
	PxTransform				initTransform;//飞机初始位置与姿态
	PxTransform				emitTransform;//炮弹发射口相对于飞机位置
	PxVec3					headForward, currentHeadForward;//初始机头朝向、当前机头朝向
	PxVec3					backForward, currentBackForward;//初始机背朝向、当前机背朝向
	PxVec3					swingForward, currentSwingForward;//初始机翼朝向、当前机翼朝向
	vector<bool>			turningState;//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
	int						rollingAngel = 0;//滚转角
	int						pitchingAngel = 0;//俯仰角
	int						currentAngel_x = 0;//当前姿态在水平面转过的角度
	int						currentAngel_z = 0;//当前姿态在垂直面转过的角度 
	int						emitBulletTime = 0;//发射间隔计时器
	float					veclocity = 8.0f;//默认飞行速度
	float					emitVeclocity = 24.0f;//默认炮弹飞行速度
	void*					user_data;//信息


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
	void controlAirPlane();
	void manualControlAirPlane();
	void manualControlAirPlane2();
	void manualControlAirPlane3();
	PxQuat getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront);
	void emit();

	//重写
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress();
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick() {};
};