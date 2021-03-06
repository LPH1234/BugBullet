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
enum MEDIA_STATE {
	init,
	mainmenu,
	pause,
	started,
	over
};
struct Game
{
	Game(GAME_STATE state, MEDIA_STATE _mediastate, float w, float h, float lx, float ly, float dt, float lt, bool firstMouse) {
		this->state = state; this->MediaState = _mediastate; this->SCR_WIDTH = w; this->SCR_HEIGHT = h; this->lastX = lx; this->lastY = ly; this->deltaTime = dt; this->lastFrame = lt; this->firstMouse = firstMouse;
		this->pause = false; this->isInit = false;
	}
	GAME_STATE state;
	MEDIA_STATE MediaState;
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
		MAP, TOWER, PLANE, TANK, TANK_BULLET, TOWER_BULLET, PLANE_BULLET = (6), PLANE_MISSLE = (50)
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

	UserData(BaseCharacter* _basecha, int input, std::string a, DATATYPE::TRIGGER_TYPE _type2) {
		basecha = _basecha;
		id = input;
		name = a;
		type2 = _type2;
	}
	UserData(BaseCharacter* _basecha, int input, std::string a, DATATYPE::ACTOR_TYPE _type, DATATYPE::TRIGGER_TYPE _type2) {
		basecha = _basecha;
		id = input;
		name = a;
		type = _type;
		type2 = _type2;
	}
	UserData(BaseSceneObject* _basesce, int input, std::string a, DATATYPE::ACTOR_TYPE _type) {
		basesce = _basesce;
		id = input;
		name = a;
		type = _type;
	}
	UserData(BaseSceneObject* _basesce, int input, std::string a, DATATYPE::ACTOR_TYPE _type, DATATYPE::TRIGGER_TYPE _type2) {
		basesce = _basesce;
		id = input;
		name = a;
		type = _type;
		type2 = _type2;
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