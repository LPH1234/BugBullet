#pragma once
#include "PxPhysicsAPI.h"
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

extern PxFoundation*			gFoundation;
extern PxPhysics*				gPhysics;
extern PxCooking*				gCooking;
extern PxDefaultCpuDispatcher*	gDispatcher;
extern PxScene*					gScene;
extern PxMaterial*				gMaterial;
extern PxPvd*					gPvd;

class AirPlane {
public:
	PxVec3					headForward;//��ͷ����
	PxVec3					backForward;//��������
	PxVec3					swingForward;//������
	vector<bool>			turningState;//�ɻ�ת���3��״̬���󷭹����ҷ�����ֱ���С��������¸�
	int						rollingAngel;//��ת��
	int						pitchingAngel;//������
	PxTransform				initTransform;//��ʼλ������̬
	PxRigidDynamic*			body;//�ɻ�����
	void*					user_data;//��Ϣ
	int						currentAngel_x = 0;//��ǰ��̬��ˮƽ��ת���ĽǶ�
	int						currentAngel_z = 0;//��ǰ��̬�ڴ�ֱ��ת���ĽǶ� 
	int						emitBulletTime = 0;//��������ʱ��
	float					veclocity = 8.0f;//Ĭ�Ϸ����ٶ�


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
	void controlAirPlane();
	void manualControlAirPlane();
};