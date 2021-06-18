#pragma once


#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include<unordered_map>

#include "PxPhysicsAPI.h"

#include "../Render/Render.h"
#include "../Render/Camera.h"

#include "../Render/models.h"
#include "../Data/Consts.h"
#include <ctime>


extern PxRigidDynamic* player_ctl;
extern std::unordered_map<int, bool> keyToPressState;
extern std::unordered_map<int, bool> keyToPrePressState;
extern Camera camera;

extern PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
extern void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);
void keypress();
void mouseMove();



void vehicleProcessKeyboard();
void playerProcessKeyboard();





// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerMovement {
	Player_FORWARD,
	Player_BACKWARD,
	Player_LEFT,
	Player_RIGHT
};


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Player
{
public:
	

private:

};
#endif
