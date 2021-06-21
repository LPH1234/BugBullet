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
	THIRD_PERSON
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
	float track_radius = 5.f;
	const float MIN_TRACK_RADIUS = 4.f;
	const float MAX_TRACK_RADIUS = 64.f;
	glm::vec3 target_position = glm::vec3(0.f, 0.f, 0.f);
	physx::PxRigidDynamic* target = nullptr;
public:

	// constructor with vectors
	Camera(VIEW_TYPE mode, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), Zoom(ZOOM)
	{
		this->mode = mode;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Movement direction, float deltaTime)
	{
		if (this->mode == VIEW_TYPE::FREE) {
			float velocity = movementSpeed * deltaTime;
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			if (direction == UP)
				Position += glm::vec3(0, 1, 0) * velocity;
			if (direction == DOWN)
				Position += glm::vec3(0, -1, 0) * velocity;
			if (direction == SHIFT_PRESS)
				movementSpeed = SPEED * 2;
			if (direction == SHIFT_RELEASE)
				movementSpeed = SPEED;
		}
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		float x_sensitivity, y_sensitivity;
		if (this->mode == VIEW_TYPE::FREE) {
			x_sensitivity = FREE_X_SENSITIVITY;
			y_sensitivity = FREE_Y_SENSITIVITY;
		}
		if (this->mode == VIEW_TYPE::THIRD_PERSON) {
			x_sensitivity = THIRD_X_SENSITIVITY;
			y_sensitivity = THIRD_Y_SENSITIVITY;
		}
		if (this->mode == VIEW_TYPE::FIRST_PERSON) {
			x_sensitivity = FIRST_X_SENSITIVITY;
			y_sensitivity = FIRST_Y_SENSITIVITY;
		}
		xoffset *= x_sensitivity;
		yoffset *= y_sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;
		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		//updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (this->mode == VIEW_TYPE::THIRD_PERSON) {
			if (yoffset + this->track_radius <= MIN_TRACK_RADIUS) {
				track_radius = MIN_TRACK_RADIUS;
				return;
			}
			if (yoffset + this->track_radius >= MAX_TRACK_RADIUS) {
				track_radius = MAX_TRACK_RADIUS;
				return;
			}
			this->track_radius += yoffset;
		}

		//Zoom -= (float)yoffset;
		/*if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;*/
	}

	void trackDynamicPosition() { // 设置相机跟随物体
		if (this->mode != VIEW_TYPE::FREE) {
			if (this->target == nullptr)
				return;
			if (IsBadWritePtr(this->target, 1) != 0) {
				std::cout << "不可访问指针：dynamic target\n";
				return;
			}
			pxVec3ToGlmVec3(target->getGlobalPose().p, this->target_position);
			if (this->mode == VIEW_TYPE::THIRD_PERSON) {
				Position.y = target_position.y - (Pitch / 180.f) * track_radius;
				float curr_radians = sqrtf(track_radius * track_radius - (target_position.y - Position.y) * (target_position.y - Position.y));
				Position.x = target_position.x + cos(Yaw) * curr_radians;
				Position.z = target_position.z + sin(Yaw) * curr_radians;
			}
			if (this->mode == VIEW_TYPE::FIRST_PERSON) {
				Position.y = target_position.y + 1.0f;
				Position.x = target_position.x;
				Position.z = target_position.z;
			}
		}
		updateCameraVectors();
	}

	void setMode(VIEW_TYPE type) {
		this->mode = type;
	}

	VIEW_TYPE getMode() {
		return this->mode;
	}

	bool isHandling() { //当Mode为FREE时，返回true
		return this->mode == VIEW_TYPE::FREE;
	}

	void setTarget(PxRigidDynamic* t) {
		this->target = t;
	}

	PxRigidDynamic* getTarget() {
		return this->target;
	}

	void setTargetPosition(glm::vec3 t) {
		this->target_position = t;
	}

	glm::vec3 getTargetPosition() {
		return this->target_position;
	}

	void setPosition(glm::vec3 p) {
		this->Position = p;
	}
	glm::vec3 getPosition() {
		return this->Position;
	}
	glm::vec3 getFront() {
		return this->Front;
	}
	glm::vec3 getRight() {
		return this->Right;
	}
	glm::vec3 getUp() {
		return this->Up;
	}
	float getZoom() {
		return this->Zoom;
	}


private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		if (this->mode == VIEW_TYPE::FREE) {
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
		if (this->mode == VIEW_TYPE::THIRD_PERSON) {
			//切面方程：(x0-a)(x-a)+(y0-b)(y-b)+(z0-c)(z-c) = R*R  
			front.x = target_position.x - Position.x;
			front.y = target_position.y - Position.y;
			front.z = target_position.z - Position.z;
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));

			//glm::vec3 up_tmp(front.x, (track_radius*track_radius - (Position.x - target_position.x)*(front.x - target_position.x) - (Position.z - target_position.z)*(front.z - target_position.z)) / (Position.y - target_position.y) + target_position.y, front.z);
			//Up = glm::normalize(up_tmp);
			//Right = glm::normalize(glm::cross(Front, Up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.

		}
		if (this->mode == VIEW_TYPE::FIRST_PERSON) {
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	}
};

#endif