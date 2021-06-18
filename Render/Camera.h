#pragma once


#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include<iostream>

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

const int VIEW_FREE = 0;
const int VIEW_FIRST_PERSON = 1;
const int VIEW_THIRD_PERSON = 2;

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

	VIEW_TYPE view_type;
	float track_radius = 5.f;
	glm::vec3 target;
public:

	// constructor with vectors
	Camera(VIEW_TYPE view_type, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), Zoom(ZOOM)
	{
		this->view_type = view_type;
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(VIEW_TYPE view_type, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), Zoom(ZOOM)
	{
		this->view_type = view_type;
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
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
		if (this->view_type == VIEW_TYPE::FREE) {
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
		if (this->view_type == VIEW_TYPE::FREE) {
			x_sensitivity = FREE_X_SENSITIVITY;
			y_sensitivity = FREE_Y_SENSITIVITY;
		}
		if (this->view_type == VIEW_TYPE::THIRD_PERSON) {
			x_sensitivity = THIRD_X_SENSITIVITY;
			y_sensitivity = THIRD_Y_SENSITIVITY;
		}
		if (this->view_type == VIEW_TYPE::FIRST_PERSON) {
			x_sensitivity = FIRST_X_SENSITIVITY;
			y_sensitivity = FIRST_Y_SENSITIVITY;
		}
		xoffset *= x_sensitivity;
		yoffset *= y_sensitivity;
		//	std::cout << "xoffset:" << xoffset << "\n";
		//	std::cout << "yoffset:" << yoffset << "\n";

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

		//	std::cout << "Yaw:" << Yaw << "\n";
		//	std::cout << "Pitch:" << Pitch << "\n";

		if (this->view_type == VIEW_TYPE::THIRD_PERSON) {
			Position.y = target.y + (Pitch / 180.f) * track_radius;
			float curr_radians = sqrtf(track_radius * track_radius - Position.y * Position.y);
			//	std::cout << "curr_radius:" << curr_radians << "\n";

			Position.x = cos(Yaw) * curr_radians;
			Position.z = sin(Yaw) * curr_radians;
			//	std::cout << "x:" << Position.x << "   z:" << Position.z << "\n";
		}
		if (this->view_type == VIEW_TYPE::FIRST_PERSON) {

		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		/*if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;*/
	}

	void setTarget(glm::vec3 t) {
		this->target = t;
	}

	glm::vec3 getTarget() {
		return this->target;
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
	float getZoom() {
		return this->Zoom;
	}


private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		if (this->view_type == VIEW_TYPE::FREE) {
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
		if (this->view_type == VIEW_TYPE::THIRD_PERSON) {
			//ÇÐÃæ·½³Ì£º(x0-a)(x-a)+(y0-b)(y-b)+(z0-c)(z-c) = R*R  
			front.x = target.x - Position.x;
			front.y = target.y - Position.y;
			front.z = target.z - Position.z;
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			//glm::vec3 up_tmp(front.x, (track_radius*track_radius-(Position.x-target.x)*(front.x-target.x)-(Position.z - target.z)*(front.z - target.z))/(Position.y-target.y)+ target.y, front.z);
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
		if (this->view_type == VIEW_TYPE::FIRST_PERSON) {

		}
	}
};
#endif