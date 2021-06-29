#include "Camera.h"
#include"../Main/Characters.h"



// constructor with vectors
Camera::Camera(VIEW_TYPE mode, glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), Zoom(ZOOM)
{
	this->mode = mode;
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Movement direction, float deltaTime)
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
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	float x_sensitivity = 0.f, y_sensitivity = 0.f;
	if (this->mode == VIEW_TYPE::FREE) {
		x_sensitivity = FREE_X_SENSITIVITY;
		y_sensitivity = FREE_Y_SENSITIVITY;
	}
	if (this->mode == VIEW_TYPE::FIRST_PERSON) {
		x_sensitivity = FIRST_X_SENSITIVITY;
		y_sensitivity = FIRST_Y_SENSITIVITY;
	}
	if (this->mode == VIEW_TYPE::THIRD_PERSON) {
		x_sensitivity = THIRD_X_SENSITIVITY;
		y_sensitivity = THIRD_Y_SENSITIVITY;
	}
	if (this->mode == VIEW_TYPE::BEHIND_PERSON_TRACK_ALL_DIRECTION) {
		x_sensitivity = THIRD_X_SENSITIVITY;
		y_sensitivity = THIRD_Y_SENSITIVITY;
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
void Camera::ProcessMouseScroll(float yoffset)
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
	if (this->mode == VIEW_TYPE::BEHIND_PERSON_TRACK_ALL_DIRECTION) {
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

void Camera::trackDynamicPosition() { // 设置相机跟随物体
	if (this->mode != VIEW_TYPE::FREE) {
		if (this->target == nullptr)
			return;
		if (IsBadWritePtr(this->target, 1) != 0) {
			std::cout << "不可访问指针：dynamic target\n";
			return;
		}
		pxVec3ToGlmVec3(target->getRigid()->getGlobalPose().p, this->target_position);
		if (this->mode == VIEW_TYPE::FIRST_PERSON) {
			Position.y = target_position.y + 1.0f;
			Position.x = target_position.x;
			Position.z = target_position.z;
		}
		if (this->mode == VIEW_TYPE::THIRD_PERSON) {
			Position.y = target_position.y - (Pitch / 180.f) * track_radius;
			float curr_radians = sqrtf(track_radius * track_radius - (target_position.y - Position.y) * (target_position.y - Position.y));
			Position.x = target_position.x + cos(Yaw) * curr_radians;
			Position.z = target_position.z + sin(Yaw) * curr_radians;
		}
		if (this->mode == VIEW_TYPE::BEHIND_PERSON_TRACK_ALL_DIRECTION) {
			PxVec3 t_f; this->target->getFront(t_f);
			PxVec3 t_u; this->target->getUp(t_u);
			glm::vec3 t_f1; pxVec3ToGlmVec3(t_f - 0.3f * t_u, t_f1);
			//glm::vec3 t_f1; pxVec3ToGlmVec3(t_f - Pitch / 180.f * t_u, t_f1);
			Position.y = target_position.y - t_f1.y * track_radius;
			float curr_radians = sqrtf(track_radius * track_radius - (target_position.y - Position.y) * (target_position.y - Position.y));
			std::cout << "curr_radis:" << curr_radians << "\n";
			Position.x = target_position.x - t_f1.x * curr_radians;
			Position.z = target_position.z - t_f1.z * curr_radians;
		}
	}
	updateCameraVectors();
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
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
	if (this->mode == VIEW_TYPE::FIRST_PERSON) {
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
	if (this->mode == VIEW_TYPE::BEHIND_PERSON_TRACK_ALL_DIRECTION) {

		front.x = target_position.x - Position.x;
		front.y = target_position.y - Position.y;
		front.z = target_position.z - Position.z;
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));
		PxVec3 Right_t(Right.x, Right.y, Right.z); this->target->getRight(Right_t);
		Right = glm::normalize(pxVec3ToGlmVec3(Right_t));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));

	}
}


void Camera::setMode(VIEW_TYPE type) {
	this->mode = type;
}

VIEW_TYPE Camera::getMode() {
	return this->mode;
}

bool Camera::isHandling() { //当Mode为FREE时，返回true
	return this->mode == VIEW_TYPE::FREE;
}

void Camera::setTarget(BaseCharacter* t) {
	this->target = t;
}

BaseCharacter* Camera::getTarget() {
	return this->target;
}

void Camera::setTargetPosition(glm::vec3 t) {
	this->target_position = t;
}

glm::vec3 Camera::getTargetPosition() {
	return this->target_position;
}

void Camera::setPosition(glm::vec3 p) {
	this->Position = p;
}
glm::vec3 Camera::getPosition() {
	return this->Position;
}
glm::vec3 Camera::getFront() {
	return this->Front;
}
glm::vec3 Camera::getRight() {
	return this->Right;
}
glm::vec3 Camera::getUp() {
	return this->Up;
}
float Camera::getZoom() {
	return this->Zoom;
}


