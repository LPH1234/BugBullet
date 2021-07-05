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
#include <set>
#include "Media.h"

class Player;
class AirPlane;
class MissileManager;
class AirPlane_AI;
extern std::unordered_map<int, bool> keyToPressState;
extern std::unordered_map<int, bool> keyToPrePressState;
extern bool mouseButtonPressState[3];
extern PxPhysics*				gPhysics;
extern PxScene*					gScene;
extern PxMaterial*				gMaterial;
extern Camera camera;
extern const float velocity;
extern std::set<PxRigidDynamic*> airPlaneBullet;
extern set<Player*>				updateTankList;
extern vector<AirPlane_AI*>		tempList;
extern vector<AirPlane_AI*>		AI_PlaneList;
//extern MissileManager			*ManageMissile;
extern Media MediaPlayer;
//extern void createshell(const PxTransform& t, const PxVec3& velocity);
extern unordered_map<int, BaseModel*> idToRenderModel;
extern vector<PxTransform> addCrashList;

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
	virtual void ProcessMouseClick(int button, int action) {};

	virtual void oncontact(DATATYPE::ACTOR_TYPE _type) {};
	virtual void oncontact(DATATYPE::TRIGGER_TYPE _type) {};

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
public:
	PxVec3 headforward = PxVec3(0.0f, 0.0f, 1.0f);
	PxVec3 backforward = PxVec3(0.0f, 1.0f, 0.0f);
	PxVec3 currentheadforward;
	PxVec3 currentbackforward;
	PxVec3 startPos;//路线起点
	PxVec3 endPos;//路线终点
	PxVec3 startDir;//起始方向
	bool autoshooting;//射击机制
	clock_t last = 0;
	PxVec3 born;
	vector<PxVec3> waypoint;

	PxRigidDynamic*			body;//刚体
	PxRigidDynamic*			healthBody;//血条刚体
	int health = 100;//坦克生命值
	bool alive = true;
	float healthLength = 6.0;//血条长度
	AirPlane*				airPlane;//飞机类
	float					bulletVelocity = 40.f;//默认子弹速度
	vector<bool>			turnningState;//转向状态，分别是直行中、转向中
	int						currentAngle = 0;//当前已经转过的角度
	int						initAngel = 0;//初始转过的角度
	float					velocity = 8.0f;//默认速度
	int						fireTime = 0;//发射间隔计时器


	Player(physx::PxRigidDynamic* target, AirPlane*	airplane, PxVec3 _startPos, PxVec3 _endPos, PxVec3 _startDir);
	void ProcessKeyPress();
	void fire(const PxTransform& t, const PxVec3& velocity);
	PxQuat getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront);
	int trackangle(PxVec3& start, PxVec3& destination);
	int forward(PxVec3& dir, double velocity);
	void automove();
	void autoEmit();
	void oncontact(DATATYPE::ACTOR_TYPE _type);
	void reset();
};


class AirPlane : public BaseCharacter {
public:
	PxRigidDynamic*			body;//飞机刚体
	MissileManager*			myMissileManager;//导弹管理器
	//AirPlane_AI*			targetPlane;//目标AI飞机
	vector<AirPlane_AI*>	AI_PlaneList_this;
	PxTransform				initTransform;//飞机初始位置与姿态
	PxTransform				emitTransform;//炮弹发射口相对于飞机位置
	PxVec3					headForward, currentHeadForward;//初始机头朝向、当前机头朝向
	PxVec3					backForward, currentBackForward;//初始机背朝向、当前机背朝向
	PxVec3					swingForward, currentSwingForward;//初始机翼朝向、当前机翼朝向
	PxVec3					emitDirection;//发射方向，定为前进方向下压20°
	vector<bool>			turningState;//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
	vector<bool>			turningState2;//飞机转向的7个状态，直行、左偏航、右偏航、俯冲、上仰、左翻滚、右翻滚
	int						rollingAngel = 0;//滚转角
	int						pitchingAngel = 0;//俯仰角
	int						currentAngel_x = 0;//当前姿态在水平面转过的角度
	int						currentAngel_z = 0;//当前姿态在垂直面转过的角度 
	int						emitBulletTime = 0;//发射间隔计时器
	float					veclocity = 16.0f;//默认飞行速度
	float					emitVeclocity = 64.0f;//默认炮弹飞行速度
	float					turningSpeed = 6.0f;//转向速度
	int						leftOrRight = -1;//左右交替发射,-1为左，+1为右

	bool track_mode = false;

	bool activatemissle = false;
	int health = 10000;//飞机生命值
	bool alive = true;
	int bullet_ammo = 10000;
	int missle_ammo = 0;

	void*					user_data;//信息


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body, MissileManager* _myMissileManager, vector<AirPlane_AI*>	&_AI_PlaneList);
	void controlAirPlane();
	void manualControlAirPlane();
	void manualControlAirPlane2();
	void manualControlAirPlane3();
	void manualControlAirPlane4();//WASDQE 6个按键
	PxQuat getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront);
	void emit();
	void reset();
	void crash();
	void shotdown();
	PxVec3 ifEmitMissile();
	void emitMissile();
	void updateUI();

	//重写
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress();
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick(int button, int action);
	void ProcessMouseClick();

	void oncontact(DATATYPE::ACTOR_TYPE _type);
	void oncontact(DATATYPE::TRIGGER_TYPE _type);
	void formcloud();
	void formmisslecloud();
};

class AirPlane_AI : public BaseCharacter {
public:
	PxRigidDynamic*			body;//飞机刚体
	MissileManager*			AI_MissileManager;//导弹管理器
	AirPlane*				targetPlane;//玩家飞机
	PxTransform				initTransform;//飞机初始位置与姿态
	PxTransform				emitTransform;//炮弹发射口相对于飞机位置
	PxVec3					headForward, currentHeadForward;//初始机头朝向、当前机头朝向
	PxVec3					backForward, currentBackForward;//初始机背朝向、当前机背朝向
	PxVec3					swingForward, currentSwingForward;//初始机翼朝向、当前机翼朝向
	vector<bool>			turningState;//飞机转向的5个状态，直行、左转、右转、上仰、俯冲

	int						rollingAngle = 0;//滚转角
	int						pitchingAngle = 0;//俯仰角
	int						yawingAngle = 0;//偏航角
	int						currentRollAngle = 0;//当前转过的滚转角
	int						currentPitchAngle = 0;//当前转过的俯仰角
	int						currentYawAngle = 0;//当前转过的偏航角
	int						emitBulletTime = 0;//发射间隔计时器
	int						currentTime = 0;//计时器，用于状态机切换
	int						lastEmit = 0;//导弹冷却计时

	float					veclocity = 10.0f;//默认飞行速度
	float					emitVeclocity = 64.0f;//默认炮弹飞行速度
	float					turningSpeed = 6.0f;//转向速度

	int						health = 20;//飞机生命值
	bool					alive = true;//是否存活

	AirPlane_AI(PxRigidDynamic*	_body);
	~AirPlane_AI();
	AirPlane_AI(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body, MissileManager* _AI_MissileManager, AirPlane* _targetPlane);
	void autoFlying();				//根据状态进行自动飞行控制
	void FSM(int currentState);		//有限状态机转换
	void autoEmit(int time);		//自动发射导弹攻击玩家飞机
	void oncontact(DATATYPE::ACTOR_TYPE _type);//被击中扣血
	void crash();					//被击毁
	void shotdown();

	//重写
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress() {};
	virtual void ProcessMouseMove() {};
	//virtual void ProcessMouseClick() {};
};




class MissileManager {
public:
	set<PxRigidDynamic*>		MissileList;
	set<PxRigidDynamic*>			MissileToRemoveList;
	int							count = 0;
	float						missileSpeed = 30.f;
	MissileManager();
	PxRigidDynamic* emitMissile(PxVec3 &emitPos, PxVec3 &direction, BaseCharacter* target);
	void trackingAllMissile();
	void removeMissile();
};