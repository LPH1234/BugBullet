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
//	PxRigidDynamic*			body;//飞机刚体
//	PxTransform				initTransform;//飞机初始位置与姿态
//	PxTransform				emitTransform;//炮弹发射口相对于飞机位置
//	PxVec3					headForward, currentHeadForward;//初始机头朝向、当前机头朝向
//	PxVec3					backForward, currentBackForward;//初始机背朝向、当前机背朝向
//	PxVec3					swingForward, currentSwingForward;//初始机翼朝向、当前机翼朝向
//	vector<bool>			turningState;//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
//	int						rollingAngel = 0;//滚转角
//	int						pitchingAngel = 0;//俯仰角
//	int						currentAngel_x = 0;//当前姿态在水平面转过的角度
//	int						currentAngel_z = 0;//当前姿态在垂直面转过的角度 
//	int						emitBulletTime = 0;//发射间隔计时器
//	float					veclocity = 8.0f;//默认飞行速度
//	float					emitVeclocity = 24.0f;//默认炮弹飞行速度
//	void*					user_data;//信息
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