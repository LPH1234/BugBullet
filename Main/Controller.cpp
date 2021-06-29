#include "Controller.h"


//ȫ�ֱ�����
const float velocity = 1.0f;
extern Camera camera;

extern vector<bool>	turningState;
extern AirPlane* Plane_1;
extern PxRigidDynamic* player;
extern Player* vehicle;
extern PxTransform born_pos;

extern Shader* envShader;
extern Shader* pointParticleShader;
extern Shader* smokeParticleShader;

bool autoshooting = true;//�������
clock_t last = 0;


void processOtherControlEvents() {
	mouseClick();
}

void keyPress() {

	//if (!camera.isHandling()) {
		//vehicleProcessKeyboard();
		//playerProcessKeyboard();
	//vehicle->ProcessKeyPress();
	Plane_1->ProcessKeyPress();
	//mouseClick();
//}

}


void mouseMove() {

}

void mouseClick() {
	if (mouseButtonPressState[GLFW_MOUSE_BUTTON_LEFT]) {
		return;
		PxTransform px;

		PxVec3 mDir; glmVec3ToPxVec3(camera.getFront(), mDir);
		PxVec3 mEye; glmVec3ToPxVec3(camera.getPosition(), mEye);
		PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0));

		if (viewY.normalize() < 1e-6f)
			px = PxTransform(mEye);
		else {
			PxMat33 m(mDir.cross(viewY), viewY, -mDir);
			px = PxTransform(mEye, PxQuat(m));
		}
		if (camera.getMode() == VIEW_TYPE::THIRD_PERSON) {
			px.p = player->getGlobalPose().p + glmVec3ToPxVec3(camera.getFront() * 2.f);
			createBullet(px, px.rotate(PxVec3(0, 0, -1)) * 200);
		}

	}
	if (mouseButtonPressState[GLFW_MOUSE_BUTTON_RIGHT]) { //����Ҽ�
		//createPointParticles(
		//	1000, false,
		//	new PointParticle(glm::vec3(1.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), pointParticleShader),
		//	glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f),
		//	false, 2.0, // true��ɢ��
		//	true, 20.0, // true������ٶ�
		//	20, 5,
		//	PxVec3(0.f, 0.f, 0.f), //��ʼ�ٶ�
		//	PxVec3(0.f, 0.f, 0.f)  //����
		//);

		std::vector<std::string> ts;
		/*ts.push_back("images/textures/smoke/smoke-white-1.png");
		ts.push_back("images/textures/smoke/smoke-gray.png");*/
		for (size_t i = 1; i <= 4; i++)
			ts.push_back("images/textures/smoke/smoke-gray-" + std::to_string(i) + ".png");
			createSmokeParticles(
				50, false,
				new SmokeParticle(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.f, 1.f, 0.f), smokeParticleShader, ts),
				/*glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f),*/ // init pos
		        Plane_1->getGlobalPose(),
				true, 0.05, // true��ɢ��, �������ɢ���뾶
				false, 20.0, // true������ٶȣ���������������ٶ�
				4, 1, // 20s ������ϵͳ���٣� �� 5s ��ʼ����
				PxVec3(0.f, 0.f, 0.f), //��ʼ�ٶ�
				PxVec3(0.f, 10.f, 0.f)  //����
			);


			//createPointParticles(
			//	100, false,
			//	//new PointParticle(glm::vec3(0.f, 0.f, 0.f), glm::vec3(50.f, 50.f, 50.f), glm::vec3(1.f, 1.f, 0.f), pointParticleShader),
			//	new Cube(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.01f, 0.01f, 0.01f), "", envShader, "images/textures/smoke.png"),
			//	glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f),
			//	true, 0.5,
			//	false, 20.0,
			//	20, 5,
			//	PxVec3(0.f, 0.f, 0.f),
			//	PxVec3(0.f, 10.f, 0.f)
			//);

	}
}


