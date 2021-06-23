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
