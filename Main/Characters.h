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
	PxVec3 startPos;//·�����
	PxVec3 endPos;//·���յ�
	PxVec3 startDir;//��ʼ����
	bool autoshooting;//�������
	clock_t last = 0;
	PxVec3 born;
	vector<PxVec3> waypoint;

	PxRigidDynamic*			body;//����
	PxRigidDynamic*			healthBody;//Ѫ������
	int health = 100;//̹������ֵ
	bool alive = true;
	float healthLength = 6.0;//Ѫ������
	AirPlane*				airPlane;//�ɻ���
	float					bulletVelocity = 40.f;//Ĭ���ӵ��ٶ�
	vector<bool>			turnningState;//ת��״̬���ֱ���ֱ���С�ת����
	int						currentAngle = 0;//��ǰ�Ѿ�ת���ĽǶ�
	int						initAngel = 0;//��ʼת���ĽǶ�
	float					velocity = 8.0f;//Ĭ���ٶ�
	int						fireTime = 0;//��������ʱ��


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
	PxRigidDynamic*			body;//�ɻ�����
	MissileManager*			myMissileManager;//����������
	//AirPlane_AI*			targetPlane;//Ŀ��AI�ɻ�
	vector<AirPlane_AI*>	AI_PlaneList_this;
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
	float					veclocity = 16.0f;//Ĭ�Ϸ����ٶ�
	float					emitVeclocity = 64.0f;//Ĭ���ڵ������ٶ�
	float					turningSpeed = 6.0f;//ת���ٶ�
	int						leftOrRight = -1;//���ҽ��淢��,-1Ϊ��+1Ϊ��

	bool track_mode = false;

	bool activatemissle = false;
	int health = 10000;//�ɻ�����ֵ
	bool alive = true;
	int bullet_ammo = 10000;
	int missle_ammo = 0;

	void*					user_data;//��Ϣ


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body, MissileManager* _myMissileManager, vector<AirPlane_AI*>	&_AI_PlaneList);
	void controlAirPlane();
	void manualControlAirPlane();
	void manualControlAirPlane2();
	void manualControlAirPlane3();
	void manualControlAirPlane4();//WASDQE 6������
	PxQuat getBulletRotate(PxVec3& neededFront, PxVec3& bulletFront);
	void emit();
	void reset();
	void crash();
	void shotdown();
	PxVec3 ifEmitMissile();
	void emitMissile();
	void updateUI();

	//��д
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
	PxRigidDynamic*			body;//�ɻ�����
	MissileManager*			AI_MissileManager;//����������
	AirPlane*				targetPlane;//��ҷɻ�
	PxTransform				initTransform;//�ɻ���ʼλ������̬
	PxTransform				emitTransform;//�ڵ����������ڷɻ�λ��
	PxVec3					headForward, currentHeadForward;//��ʼ��ͷ���򡢵�ǰ��ͷ����
	PxVec3					backForward, currentBackForward;//��ʼ�������򡢵�ǰ��������
	PxVec3					swingForward, currentSwingForward;//��ʼ�����򡢵�ǰ������
	vector<bool>			turningState;//�ɻ�ת���5��״̬��ֱ�С���ת����ת������������

	int						rollingAngle = 0;//��ת��
	int						pitchingAngle = 0;//������
	int						yawingAngle = 0;//ƫ����
	int						currentRollAngle = 0;//��ǰת���Ĺ�ת��
	int						currentPitchAngle = 0;//��ǰת���ĸ�����
	int						currentYawAngle = 0;//��ǰת����ƫ����
	int						emitBulletTime = 0;//��������ʱ��
	int						currentTime = 0;//��ʱ��������״̬���л�
	int						lastEmit = 0;//������ȴ��ʱ

	float					veclocity = 10.0f;//Ĭ�Ϸ����ٶ�
	float					emitVeclocity = 64.0f;//Ĭ���ڵ������ٶ�
	float					turningSpeed = 6.0f;//ת���ٶ�

	int						health = 20;//�ɻ�����ֵ
	bool					alive = true;//�Ƿ���

	AirPlane_AI(PxRigidDynamic*	_body);
	~AirPlane_AI();
	AirPlane_AI(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body, MissileManager* _AI_MissileManager, AirPlane* _targetPlane);
	void autoFlying();				//����״̬�����Զ����п���
	void FSM(int currentState);		//����״̬��ת��
	void autoEmit(int time);		//�Զ����䵼��������ҷɻ�
	void oncontact(DATATYPE::ACTOR_TYPE _type);//�����п�Ѫ
	void crash();					//������
	void shotdown();

	//��д
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