#pragma once
#ifndef _DATA_
#define _DATA_


#include<string>
struct DATATYPE {
	enum ACTOR_TYPE {
		MAP,TOWER, PLANE,TANK,TANK_BULLET,TOWER_BULLET,PLANE_BULLET
};	
	enum TRIGGER_TYPE {
		BORDER,
		COLLECTION,
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
	UserData(BaseCharacter* _basecha,int input, std::string a, DATATYPE::ACTOR_TYPE _type) {
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