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
extern Shader* cloudShader;
extern Shader* flameShader;
extern Shader* smokeShader;

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
		glm::vec3 initPos = camera.getPosition() + camera.getFront() * 2.f + camera.getUp() * 0.3f;
		//SmokeParticleCluster(int cloudDensity, float cloudRadis, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader);
		//camera.getPosition() + camera.getFront() * 1.f + camera.getUp() * 0.5f,
		////// 1������
		SmokeParticleCluster* smoke_cluster = new SmokeParticleCluster(
			100, 2.f,  // �����ܶȡ������ŵİ뾶
			90, 0.01f, 3.4f, // ÿ���������Ӵ�С��������y������ٶȡ�������y����������������ľ���
			glm::vec3(3.f, 1.5f, 3.f), //��ʼλ��
			//camera.getPosition() + camera.getFront() * 1.f,
			std::vector<string>(), // ����·���б�
			smokeShader //��Ⱦ�������shader
		);
		renderParticleClusterList.push_back(smoke_cluster);

		///////2����ը����
		//vector<string> paths;
		//for (int i = 1; i <= 18; i++) {
		//	paths.push_back("model/particle/crash/" + to_string(i) + ".obj"); //��е�к���Ƭ
		//}

		//createPointParticles(
		//	10, false,
		//	new DebrisParticle(glm::vec3(0.01f, 0.01f, 0.01f), paths, glm::vec3(1.f, 1.f, 0.f), envShader),
		//	glmVec3ToPxVec3(initPos),
		//	false, 2.0, // true��ɢ��
		//	true, 20.0, // true������ٶ�
		//	12, 5, // ��ʧʱ�䡢��ʼ����ʱ��
		//	PxVec3(10.f, 5.f, 0.f), //��ʼ�ٶ�
		//	PxVec3(2.f, 5.f, 0.f)  //����
		//);

		///////3����β��	
		///////CloudParticleCluster(int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader)
		//CloudParticleCluster* cloud_cluster = new CloudParticleCluster(
		//	100, 0.05f,  //���ܶȡ����ŵİ뾶
		//	0.01f, 3.4f, // ����y������ٶȡ�����y����������������ľ���
		//	glm::vec3(3.f, 1.5f, 3.f), //��ʼλ��
		//	glm::vec3(0.1f, 0.1f, 0.1f), //ÿƬ�����ӵ�����
		//	//camera.getPosition() + camera.getFront() * 1.f,
		//	std::vector<string>(), // ����·���б�
		//	cloudShader //��Ⱦ�������shader
		//);
		//renderParticleClusterList.push_back(cloud_cluster);


		///////4.flame
		/*FlameParticleCluster* flame_cluster = new FlameParticleCluster(5, 1.f, 5.1f, 10, initPos, std::vector<string>(), flameShader);
		renderParticleClusterList.push_back(flame_cluster);*/

		//std::vector<std::string> ts;
		////ts.push_back("images/textures/smoke/smoke-white-1.png");
		////ts.push_back("images/textures/smoke/smoke-gray.png");
		//for (int i = 1; i <= 4; i++)
		//	ts.push_back("images/textures/smoke/smoke-gray-" + std::to_string(i) + ".png");
		//	createSmokeParticles(
		//		50, false,
		//		new SmokeParticle(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.f, 1.f, 0.f), cloudShader, ts),
		//		glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f), // init pos
		//		true, 0.05, // true��ɢ��, �������ɢ���뾶
		//		false, 20.0, // true������ٶȣ���������������ٶ�
		//		20, 5, // 20s ������ϵͳ���٣� �� 5s ��ʼ����
		//		PxVec3(0.f, 0.f, 0.f), //��ʼ�ٶ�
		//		PxVec3(0.f, 10.f, 0.f)  //����
		//	);


			//createPointParticles(
			//	100, false,
			//	//new DebrisParticle(glm::vec3(0.f, 0.f, 0.f), glm::vec3(50.f, 50.f, 50.f), glm::vec3(1.f, 1.f, 0.f), pointParticleShader),
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


