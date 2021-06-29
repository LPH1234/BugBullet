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
	bool autoshooting;//�������
	clock_t last = 0;
	PxVec3 born;
	vector<PxVec3> waypoint;

	int health = 100;//̹������ֵ

public:
	PxRigidDynamic*			body;//����
	AirPlane*				airPlane;//�ɻ���
	float					bulletVelocity = 40.f;//Ĭ���ӵ��ٶ�
	vector<bool>			turnningState;//ת��״̬���ֱ���ֱ���С�ת����
	int						currentAngle = 0;//��ǰ�Ѿ�ת���ĽǶ�
	float					velocity = 2.0f;//Ĭ���ٶ�
	int						fireTime = 0;//��������ʱ��
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
	PxRigidDynamic*			body;//�ɻ�����
	PxTransform				initTransform;//�ɻ���ʼλ������̬
	PxTransform				emitTransform;//�ڵ����������ڷɻ�λ��
	PxVec3					headForward, currentHeadForward;//��ʼ��ͷ���򡢵�ǰ��ͷ����
	PxVec3					backForward, currentBackForward;//��ʼ�������򡢵�ǰ��������
	PxVec3					swingForward, currentSwingForward;//��ʼ�����򡢵�ǰ������
	vector<bool>			turningState;//�ɻ�ת���3��״̬���󷭹����ҷ�����ֱ���С��������¸�
	vector<bool>			turningState2;//�ɻ�ת���7��״̬��ֱ�С���ƫ������ƫ�������塢�������󷭹����ҷ���
	int						rollingAngel = 0;//��ת��
	int						pitchingAngel = 0;//������
	int						currentAngel_x = 0;//��ǰ��̬��ˮƽ��ת���ĽǶ�
	int						currentAngel_z = 0;//��ǰ��̬�ڴ�ֱ��ת���ĽǶ� 
	int						emitBulletTime = 0;//��������ʱ��
	float					veclocity = 8.0f;//Ĭ�Ϸ����ٶ�
	float					emitVeclocity = 24.0f;//Ĭ���ڵ������ٶ�
	float					turningSpeed = 6.0f;//ת���ٶ�
	
	int health = 100;//�ɻ�����ֵ

	void*					user_data;//��Ϣ


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
	void controlAirPlane();
	void manualControlAirPlane();
	void manualControlAirPlane2();
	void manualControlAirPlane3();
	void manualControlAirPlane4();//WASDQE 6������
	PxQuat getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront);
	void emit();
	void reset();

	//��д
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress();
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick() {};

	void oncontact(DATATYPE::ACTOR_TYPE _type);
};