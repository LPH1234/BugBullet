#pragma once


#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

extern PxRigidDynamic* player_ctl;
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


	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(PlayerMovement direction, float deltaTime)
	{
		cout << direction << endl;
		//float velocity = MovementSpeed * deltaTime;
		float velocity = MovementSpeed;
		cout << "velocity: " << velocity << endl;
		if (direction == Player_FORWARD)
		{
			PxVec3 forward_velocity(0.0f,0.0f,(-1)*velocity);
			player_ctl->setLinearVelocity(forward_velocity);
			//Position += Front * velocity;
			//Position.z -= velocity;
			//cout << "position.x: " << Position.x << "position.y: " << Position.y << "position.z: " << Position.z << endl;
			//PxTransform output(PxVec3(Position.x,Position.y,Position.z));
			//player_ctl->setGlobalPose(output);
			
		}
		if (direction == Player_BACKWARD)
		{
			PxVec3 backward_velocity(0.0f, 0.0f,velocity);
			player_ctl->setLinearVelocity(backward_velocity);
			/*Position.z +=  velocity;
			cout << "position.x: " << Position.x << "position.y: " << Position.y << "position.z: " << Position.z << endl;
			PxTransform output(PxVec3(Position.x, Position.y, Position.z));
			player_ctl->setGlobalPose(output);
			*/
		}
			
		if (direction == Player_LEFT)
		{
			PxVec3 leftward_velocity((-1)*velocity, 0.0f,0.0f);
			player_ctl->setLinearVelocity(leftward_velocity);
          /*Position.x -=  velocity;
		  cout << "position.x: " << Position.x << "position.y: " << Position.y << "position.z: " << Position.z << endl;
		  PxTransform output(PxVec3(Position.x, Position.y, Position.z));
		  player_ctl->setGlobalPose(output);
		  */

		}
			
		if (direction == Player_RIGHT)
		{
			PxVec3 rightward_velocity(velocity, 0.0f, 0.0f);
			player_ctl->setLinearVelocity(rightward_velocity);
			/*Position.x += velocity;
			cout << "position.x: " << Position.x << "position.y: " << Position.y << "position.z: " << Position.z << endl;
			PxTransform output(PxVec3(Position.x, Position.y, Position.z));
			player_ctl->setGlobalPose(output);
			*/
		}

		
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
