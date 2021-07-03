#pragma once
#ifndef _DATA_
#define _DATA_


#include<string>

enum GAME_STATE
{
	INIT,
	MAIN_MENU,
	STARTED,
	PAUSE,
	OVER
};

struct Game
{
	Game(GAME_STATE state, float w, float h, float lx, float ly, float dt, float lt, bool firstMouse) {
		this->state = state; this->SCR_WIDTH = w; this->SCR_HEIGHT = h; this->lastX = lx; this->lastY = ly; this->deltaTime = dt; this->lastFrame = lt; this->firstMouse = firstMouse;
		this->pause = false; this->isInit = false;
	}
	GAME_STATE state;
	float SCR_WIDTH;
	float SCR_HEIGHT;
	float lastX;
	float lastY;
	float deltaTime;
	float lastFrame;
	bool firstMouse;
	bool pause;
	bool isInit;
};


struct DATATYPE {
	enum ACTOR_TYPE {
		MAP, TOWER, PLANE, TANK, TANK_BULLET, TOWER_BULLET, PLANE_BULLET, PLANE_MISSLE = (20)
	};
	enum TRIGGER_TYPE {
		BORDER,
		COLLECTION,
		BLOOD,
		SUPPLY
	};
};
class BaseCharacter;
class BaseSceneObject;
struct UserData
{
	int id;
	std::string name;
	DATATYPE::ACTOR_TYPE type;
	DATATYPE::TRIGGER_TYPE type2;
	BaseCharacter* basecha = nullptr;
	BaseSceneObject* basesce = nullptr;
	UserData(BaseCharacter* _basecha, int input, std::string a, DATATYPE::ACTOR_TYPE _type) {
		basecha = _basecha;
		id = input;
		name = a;
		type = _type;
	}
	UserData(BaseSceneObject* _basesce, int input, std::string a, DATATYPE::ACTOR_TYPE _type) {
		basesce = _basesce;
		id = input;
		name = a;
		type = _type;
	}
	UserData(BaseSceneObject* _basesce, int input, std::string a, DATATYPE::TRIGGER_TYPE _type) {
		basesce = _basesce;
		id = input;
		name = a;
		type2 = _type;
	}
	UserData(int input, std::string a, DATATYPE::ACTOR_TYPE _type) {
		id = input;
		name = a;
		type = _type;
	}
	UserData(int input, std::string a, DATATYPE::TRIGGER_TYPE _type) {
		id = input;
		name = a;
		type2 = _type;
	}
};

#endif