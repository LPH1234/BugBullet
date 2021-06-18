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

// Default camera values
const float player_YAW = -90.0f;
const float player_PITCH = 0.0f;
const float player_SPEED = 5.0f;
const float player_SENSITIVITY = 0.1f;
const float player_ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Player
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// constructor with vectors
	Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = player_YAW, float pitch = player_PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(player_SPEED), MouseSensitivity(player_SENSITIVITY), Zoom(player_ZOOM)
	{
		cout << "trigger" << endl;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Player(float posX, float posY, float posZ, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = player_YAW, float pitch = player_PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(player_SPEED), MouseSensitivity(player_SENSITIVITY), Zoom(player_ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		cout << " posX: " << posX << " poxY: " << posY << " posZ: " << posZ << endl;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}


	

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
