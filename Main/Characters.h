#pragma once
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
#include "Creater.h"

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

	virtual void oncontact(DATATYPE::ACTOR_TYPE _type) {};

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

class AirPlane;

class Player : public BaseCharacter {
private:
	PxVec3 headforward = PxVec3(0.0f, 0.0f, 1.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 currentheadforward;
	PxVec3 currentbackforward;
	bool autoshooting;//射击机制
	clock_t last = 0;
	PxVec3 born;
	vector<PxVec3> waypoint;

	int health = 100;//坦克生命值

public:
	PxRigidDynamic*			body;//刚体
	AirPlane*				airPlane;//飞机类
	float					bulletVelocity = 40.f;//默认子弹速度
	vector<bool>			turnningState;//转向状态，分别是直行中、转向中
	int						currentAngle = 0;//当前已经转过的角度
	float					velocity = 2.0f;//默认速度
	int						fireTime = 0;//发射间隔计时器
	Player(physx::PxRigidDynamic* target, AirPlane*	airplane);
	void ProcessKeyPress();
	void fire(const PxTransform& t, const PxVec3& velocity);
	PxQuat getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront);
	int trackangle(PxVec3& start, PxVec3& destination);
	int forward(PxVec3& dir, double velocity);
	void automove();
	void autoEmit();
	void oncontact(DATATYPE::ACTOR_TYPE _type);
};



class AirPlane : public BaseCharacter {
public:
	PxRigidDynamic*			body;//飞机刚体
	PxTransform				initTransform;//飞机初始位置与姿态
	PxTransform				emitTransform;//炮弹发射口相对于飞机位置
	PxVec3					headForward, currentHeadForward;//初始机头朝向、当前机头朝向
	PxVec3					backForward, currentBackForward;//初始机背朝向、当前机背朝向
	PxVec3					swingForward, currentSwingForward;//初始机翼朝向、当前机翼朝向
	vector<bool>			turningState;//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
	vector<bool>			turningState2;//飞机转向的7个状态，直行、左偏航、右偏航、俯冲、上仰、左翻滚、右翻滚
	int						rollingAngel = 0;//滚转角
	int						pitchingAngel = 0;//俯仰角
	int						currentAngel_x = 0;//当前姿态在水平面转过的角度
	int						currentAngel_z = 0;//当前姿态在垂直面转过的角度 
	int						emitBulletTime = 0;//发射间隔计时器
	float					veclocity = 8.0f;//默认飞行速度
	float					emitVeclocity = 24.0f;//默认炮弹飞行速度
	float					turningSpeed = 6.0f;//转向速度
	
	int health = 100;//飞机生命值

	void*					user_data;//信息


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
	void controlAirPlane();
	void manualControlAirPlane();
	void manualControlAirPlane2();
	void manualControlAirPlane3();
	void manualControlAirPlane4();//WASDQE 6个按键
	PxQuat getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront);
	void emit();
	void reset();

	//重写
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress();
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick() {};

	void oncontact(DATATYPE::ACTOR_TYPE _type);
};