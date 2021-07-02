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


class Player;
extern std::unordered_map<int, bool> keyToPressState;
extern std::unordered_map<int, bool> keyToPrePressState;
extern bool mouseButtonPressState[3];
extern PxPhysics*				gPhysics;
extern PxScene*					gScene;
extern PxMaterial*				gMaterial;
extern Camera camera;
extern PxTransform born_pos;
extern const float velocity;
extern std::set<PxRigidDynamic*> airPlaneBullet;
extern set<Player*>		updateTankList;

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
	//����Ѫ��,����Ϊ�����󶨵����塢Ѫ�����ȡ�Ѫ��λ�á�joint����������λ���Լ�joint�����Ѫ����λ��
	PxRigidDynamic* createAndShowBlood(PxRigidDynamic* _body, float _healthLength, PxTransform _healthPos, PxTransform t0, PxTransform t1) {
		PxShape* healthShape = gPhysics->createShape(PxBoxGeometry(_healthLength / 2, 0.1f, 0.1f), *gMaterial, true);
		PxRigidDynamic* bloodDynamic = PxCreateDynamic(*gPhysics, _healthPos, *healthShape, 0.0001);
		bloodDynamic->setName("blood");
		bloodDynamic->userData = new UserData(0, "blood", DATATYPE::TRIGGER_TYPE::BLOOD);
		healthShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		healthShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		bloodDynamic->attachShape(*healthShape);
		bloodDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		PxFixedJointCreate(*gPhysics, _body, t0, bloodDynamic, t1);
		return bloodDynamic;
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


public:
	PxRigidDynamic*			body;//����
	PxRigidDynamic*			healthBody;//Ѫ������
	int health = 100;//̹������ֵ
	bool alive = true;
	float healthLength = 6.0;//Ѫ������
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
	PxVec3					emitDirection;//���䷽�򣬶�Ϊǰ��������ѹ20��
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
	int						leftOrRight = -1;//���ҽ��淢��,-1Ϊ��+1Ϊ��

	bool activatemissle = false;
	int health = 100;//�ɻ�����ֵ
	bool alive = true;
	int bullet_ammo = 100;
	int missle_ammo = 0;

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
	void crash();

	//��д
	virtual void getRight(physx::PxVec3& right);
	virtual void getFront(physx::PxVec3& front);
	virtual void getUp(physx::PxVec3& up);
	virtual void ProcessKeyPress();
	virtual void ProcessMouseMove() {};
	virtual void ProcessMouseClick() {};

	void oncontact(DATATYPE::ACTOR_TYPE _type);
	void oncontact(DATATYPE::TRIGGER_TYPE _type);
	void formcloud();
	void formmisslecloud();
};