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
extern PxScene*				gScene;
extern PxMaterial*				gMaterial;
extern PxPvd*                  gPvd;