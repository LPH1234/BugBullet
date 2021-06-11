#pragma once
#include <ctype.h>

#include "PxPhysicsAPI.h"
#include "../Common/Print.h"
#include "../Common/PVD.h"
#include "../Utils/Utils.h"
#include "PxSimulationEventCallback.h"
#define PI 3.1415926

using namespace physx;

class module :public PxSimulationEventCallback {
public:
	module();
	module(PxTransform o, PxShape* s, PxTransform l, PxRigidActor* b0);
	virtual ~module();

	// Implements PxSimulationEventCallback
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onConstraintBreak(PxConstraintInfo* a, PxU32 b) { PX_UNUSED(a); PX_UNUSED(b);}
	void onWake(PxActor** a, PxU32 b);// { PX_UNUSED(a); PX_UNUSED(b); }
	void onSleep(PxActor** a, PxU32 b) { PX_UNUSED(a); PX_UNUSED(b);}
	void onAdvance(const PxRigidBody*const* a, const PxTransform* b, const PxU32 c) { 
		PX_UNUSED(a); 
		PX_UNUSED(b);
		PX_UNUSED(c);
	}


	PxTransform ori;//����ϵԭ��
	PxShape* shape;//��״
	PxTransform local;//����������ϵ�е����λ��
	PxRigidActor* body0;//����trigger���ĸ�������0��1
	//PxRigidActor* body1;
};