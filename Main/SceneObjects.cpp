#pragma once
#include "SceneObjects.h"
extern Shader* envShader;
extern PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(20));
extern PxRigidActor* createModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, bool ifStatic = true);
PxVec3 guntower::initguntower(glm::vec3 pos) {
	//glm::vec3 pos1(5.0f, 5.0f, 0.0f);

	glm::vec3 pos1(pos.x, pos.y - 0.75f, pos.z);

	PxRigidStatic* guntower = reinterpret_cast<PxRigidStatic*>(createModel(pos1, glm::vec3(0.5f, 0.5f, 0.5f), "model/vehicle/AA/flak38.obj", envShader));

	PxVec3 mPos; glmVec3ToPxVec3(pos, mPos);
	//PxTransform mDir = PxTransform(target->getGlobalPose().p-mPos);
	//PxVec3 mDir = (target->getGlobalPose().p - mPos);
	//autoattack(PxTransform(mPos), mDir);
	//guntower::towerpos = mPos;
	guntower->userData = new TowerData(1, "Tower", 50, true);
	guntower->setName("Tower");
	//guntower::towerpos_list.push_back(mPos);
	//guntower::timer_list.push_back(0);
	//guntower::tower_list.push_back(guntower_5);
	//TowerData* temp = reinterpret_cast<TowerData*>(guntower->userData);
	guntower::tower_list.push_back(guntower);
	//cout << temp->id << endl;
	return mPos;
}
void guntower::initlist(vector<glm::vec3> pos_list) {
	for (int i = 0; i < pos_list.size(); i++) {
		PxVec3 e = initguntower(pos_list[i]);
		guntower::towerpos_list.push_back(e);
		guntower::timer_list.push_back(0);
	}
}

void guntower::autoattack(PxRigidDynamic* target, PxVec3 pos) {
	PxVec3 velocity = (target->getGlobalPose().p - pos);
	createDynamic(PxTransform(pos), PxSphereGeometry(0.1f), velocity);
	//cout << "gunshot" << endl;
}
void guntower::rotate(PxRigidDynamic* target,PxVec3 current) {
	PxVec3 target_pos = PxVec3(target->getGlobalPose().p.x,current.y, target->getGlobalPose().p.z);
	PxVec3 dir = target_pos - current;
	cout << dir.x << " " << dir.y << " " << dir.z << endl;
	
	double angle = acos((dir.getNormalized()).dot(this->currentheadforward.getNormalized()));
	PxQuat rot(angle, currentbackforward);
}
void guntower::runguntower(PxRigidDynamic* target) {
	//clock_t timer_now = clock();
	for (int i = 0; i < towerpos_list.size(); i++) {
		PxVec3 e = towerpos_list[i];
		//cout << tower_list[i] << endl;
		TowerData* temp = reinterpret_cast<TowerData*>(tower_list[i]->userData);
		//cout << tower_list[i]->enable_attacking << endl;
		if (temp->enable_attacking == true) {
			//cout << temp->enable_attacking << endl;
			clock_t timer_now = clock();
			if (timer_now - timer_list[i] > 1000) {
				autoattack(target, e);
				timer_list[i] = timer_now;
			}
		}

	}

	/*PxVec3 e= guntower::towerpos;
	clock_t timer_now = clock();
	if (timer_now - timer_last > 1000) {
		autoattack(target, e);
		timer_last = timer_now;
	}*/

}

