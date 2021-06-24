#pragma once
#include<string>

struct UserData
{
	int id;
	std::string name;
	int health;
	int ammo;
	UserData(int input, std::string a, int hea,int am) {
		id = input;
		name = a;
		health = hea;
		ammo = am;
	}
};
struct PlaneData {
	int id;
	std::string name;
	int health;
};
struct TowerData {
	int id;
	std::string name;
	int health;
	bool enable_attacking;
	TowerData(int _id, std::string _name, int _hea,bool _enable) {
		id = _id;
		name = _name;
		health = _hea;
		enable_attacking = _enable;
	}
};
struct BulletData {
	int id;
	std::string name;
	int damage;

};
struct BombData {
	int id;
	std::string name;
	int damage;
};
struct enemy_Bomb_Data {
	int id;
	std::string name;
	int damage;
};
