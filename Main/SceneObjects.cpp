#pragma once
#include "SceneObjects.h"


extern Shader* envShader;

PxVec3 guntower::initguntower(glm::vec3 pos) {
	//glm::vec3 pos1(5.0f, 5.0f, 0.0f);

	glm::vec3 pos1(pos.x, pos.y - 0.75f, pos.z);

	PxRigidDynamic* guntower = reinterpret_cast<PxRigidDynamic*>(createModel(pos1, glm::vec3(0.5f, 0.5f, 0.5f), "model/vehicle/AA/flak38.obj", envShader,false));

	PxVec3 mPos; glmVec3ToPxVec3(pos, mPos);
	
	
	/*guntower->userData = new TowerData(1, "Tower", 50, true,DATATYPE::ACTOR_TYPE::TOWER);*/
	guntower->userData = new UserData(this, count, "tower", DATATYPE::ACTOR_TYPE::TOWER);
	count++;
	guntower->setName("Tower");
	
	guntower::health_list.push_back(50);
	guntower::enable_attack_list.push_back(true);
	setupFiltering(guntower, FilterGroup::eTower, FilterGroup::eMISILE );
	
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
PxQuat guntower::getshellrotate(const PxVec3& needfront, const PxVec3& bulletfront) {
	/*cout << "neededFront:" << needfront.x << "\t" << needfront.y << "\t" << needfront.z << "\t\t"
		<< "bulletFront:" << bulletfront.x << "\t" << bulletfront.y << "\t" << bulletfront.z << "\n";*/
	double cosine = needfront.getNormalized().dot(bulletfront.getNormalized());
	float angelRadius = acos(cosine);
	PxVec3 rotateAxis = bulletfront.cross(needfront);
	PxQuat rot(angelRadius, rotateAxis.getNormalized());
	return rot;
}
void guntower::fire(const PxTransform& t, const PxVec3& velocity) {
	PxQuat bulletRot(-PxPi / 2, PxVec3(0, 1, 0));
	/*emitTransform.q = getBulletRotate(t, PxVec3(1.0f, 0.0f, 0.0f));*/
	//PxTransform emitTransform = PxTransform(currentbackforward * 1);
	PxTransform emitTransform = t;
	/*emitTransform.q = bulletRot;*/
	emitTransform.q = getshellrotate(velocity, PxVec3(1.0f, 0.0f, 0.0f));
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, emitTransform, PxCapsuleGeometry(0.04, 0.07), *gMaterial, 1.0f);
	//���ø�������
	dynamic->setName("bullet");
	dynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	dynamic->setActorFlag(PxActorFlag::eVISUALIZATION, true);
	dynamic->setLinearVelocity(velocity);
	
	setupFiltering(dynamic, FilterGroup::eTowerBullet, FilterGroup::ePlayer);

	dynamic->userData = new UserData(1, "ab",DATATYPE::ACTOR_TYPE::TOWER_BULLET);
	UserData* temp = reinterpret_cast<UserData*>(dynamic->userData);
	//cout << temp->id << endl;
	//cout << a << endl;
	gScene->addActor(*dynamic);
}
void guntower::autoattack(PxRigidDynamic* target, PxVec3 pos) {
	PxVec3 target_pos = target->getGlobalPose().p;
	double distance = sqrt(pow(target_pos.x - pos.x, 2) + pow(target_pos.y - pos.y, 2) + pow(target_pos.z - pos.z, 2));
	if (distance > 200.0f) {
		cout << "out of range" << endl;
		return;
	}
	PxVec3 velocity = (target_pos - pos);
	fire(PxTransform(pos), velocity);
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
	for (int i = 0; i < towerpos_list.size(); i++) {
		PxVec3 e = towerpos_list[i];
		if (enable_attack_list[i] == true) {
			clock_t timer_now = clock();
			if (timer_now - timer_list[i] > 3000) {
				autoattack(target, e);
				timer_list[i] = timer_now;
			}
		}

	}
}

void guntower::oncontact(int id,DATATYPE::ACTOR_TYPE _type) {
	int damage = int(_type) * 2;
	if (this->health_list[id]- damage > 0) {
		this->health_list[id] -= damage;
		cout << "Tower - " << damage << endl;
	}
	else {
		this->enable_attack_list[id] = false;
		cout << "Tower died" << endl;
	}
}