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
	PxVec3					headForward;//机头朝向
	PxVec3					backForward;//机背朝向
	PxVec3					swingForward;//机翼朝向
	vector<bool>			turningState;//飞机转向的3个状态，左翻滚、右翻滚、直行中、上仰、下俯
	int						rollingAngel;//滚转角
	int						pitchingAngel;//俯仰角
	PxTransform				initTransform;//初始位置与姿态
	PxRigidDynamic*			body;//飞机刚体
	void*					user_data;//信息
	int						currentAngel_x = 0;//当前姿态在水平面转过的角度
	int						currentAngel_z = 0;//当前姿态在垂直面转过的角度 
	int						emitBulletTime = 0;//发射间隔计时器
	float					veclocity = 8.0f;//默认飞行速度


	AirPlane();
	~AirPlane();
	AirPlane(PxVec3 head, PxVec3 back, PxVec3 swing, PxRigidDynamic* _body);
	void controlAirPlane();
	void manualControlAirPlane();
};