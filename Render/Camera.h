#pragma once


#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PxRigidDynamic.h"
#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"

#include "../Utils/Convert.h"

#include <vector>
#include <iostream>
#include <windows.h>

using namespace physx;
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	SHIFT_PRESS,
	SHIFT_RELEASE,
};

enum VIEW_TYPE {
	FREE,
	FIRST_PERSON,
	THIRD_PERSON,
	BEHIND_PERSON_TRACK_ALL_DIRECTION,
};


// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float ZOOM = 45.0f;

const float FREE_X_SENSITIVITY = 0.1;
const float FREE_Y_SENSITIVITY = 0.1;
const float THIRD_X_SENSITIVITY = 0.0005;
const float THIRD_Y_SENSITIVITY = 0.1;
const float FIRST_X_SENSITIVITY = 0.1;
const float FIRST_Y_SENSITIVITY = 0.1;

//声明类
class BaseCharacter;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float movementSpeed;
	float mouseSensitivity;
	float Zoom;

	VIEW_TYPE mode;
	float track_radius = 12.f; //默认相机追踪包围球半径
	const float MIN_TRACK_RADIUS = 4.f;
	const float MAX_TRACK_RADIUS = 64.f;
	glm::vec3 target_position = glm::vec3(0.f, 0.f, 0.f);
	//physx::PxRigidDynamic* target = nullptr;
	BaseCharacter* target = nullptr;
public:

	// constructor with vectors
	Camera(VIEW_TYPE mode, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Movement direction, float deltaTime);
	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);
	void trackDynamicPosition();
	void setMode(VIEW_TYPE type);
	VIEW_TYPE getMode();
	bool isHandling();
	void setTarget(BaseCharacter* t);
	BaseCharacter* getTarget();
	void setTargetPosition(glm::vec3 t);
	glm::vec3 getTargetPosition();
	void setPosition(glm::vec3 p);
	glm::vec3 getPosition();
	glm::vec3 getFront();
	glm::vec3 getRight();
	glm::vec3 getUp();
	float getZoom();


private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();

};

#endif