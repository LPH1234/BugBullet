//#pragma once
//#include "PxPhysicsAPI.h"
//#include "../Common/Print.h"
//#include "../Common/PVD.h"
//#include "../Utils/Utils.h"
//#include "../Render/objLoader.h"
//#include "../Utils/module.h"
//#include "../Render/models.h"
//#include "../Render/Render.h"
//#include "../Data/Data.h"
//#include <cmath>
//#include "../Data/Consts.h"
//
////extern PxFoundation*			gFoundation;
//extern PxPhysics*				gPhysics;
////extern PxCooking*				gCooking;
////extern PxDefaultCpuDispatcher*	gDispatcher;
//extern PxScene*					gScene;
//extern PxMaterial*				gMaterial;
////extern PxPvd*					gPvd;
//
//class AirPlane {
//public:
//	PxRigidDynamic*			body;//�ɻ�����
//	PxTransform				initTransform;//�ɻ���ʼλ������̬
//	PxTransform				emitTransform;//�ڵ����������ڷɻ�λ��
//	PxVec3					headForward, currentHeadForward;//��ʼ��ͷ���򡢵�ǰ��ͷ����
//	PxVec3					backForward, currentBackForward;//��ʼ�������򡢵�ǰ��������
//	PxVec3					swingForward, currentSwingForward;//��ʼ�����򡢵�ǰ������
//	vector<bool>			turningState;//�ɻ�ת���3��״̬���󷭹����ҷ�����ֱ���С��������¸�
//	int						rollingAngel = 0;//��ת��
//	int						pitchingAngel = 0;//������
//	int						currentAngel_x = 0;//��ǰ��̬��ˮƽ��ת���ĽǶ�
//	int						currentAngel_z = 0;//��ǰ��̬�ڴ�ֱ��ת���ĽǶ� 
//	int						emitBulletTime = 0;//��������ʱ��
//	float					veclocity = 8.0f;//Ĭ�Ϸ����ٶ�
//	float					emitVeclocity = 24.0f;//Ĭ���ڵ������ٶ�
//	void*					user_data;//��Ϣ
//
//
//	AirPlane();
//	~AirPlane();
//	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
//	void controlAirPlane();
//	void manualControlAirPlane();
//	void manualControlAirPlane2();
//	void manualControlAirPlane3();
//	void emit();
//};