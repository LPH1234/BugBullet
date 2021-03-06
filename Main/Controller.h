#pragma once


#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include<unordered_map>
#include <ctime>

#include "PxPhysicsAPI.h"

#include "../Render/Render.h"
#include "../Render/Camera.h"

#include "../Render/Cluster.h"
#include "../Data/Consts.h"
#include "Creater.h"
#include "Characters.h"

extern std::unordered_map<int, bool> keyToPressState;
extern std::unordered_map<int, bool> keyToPrePressState;
extern bool mouseButtonPressState[3];
extern Camera camera;

void processOtherControlEvents(); // 每次渲染循环会调用这个方法
void keyPress();
void mouseMove();
void mouseClick(); //每次循环都会调用
void mouseSingleClick(int button, int action); //鼠标单次点击



//void vehicleProcessKeyboard();
//void playerProcessKeyboard();
//void planeProcessKeyboard();




// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum PlayerMovement {
	Player_FORWARD,
	Player_BACKWARD,
	Player_LEFT,
	Player_RIGHT
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL

#endif
