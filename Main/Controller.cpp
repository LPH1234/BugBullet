#include "Controller.h"


//全局变量区
const float velocity = 1.0f;
extern Camera camera;
extern Game game;

extern vector<bool>	turningState;
extern AirPlane* Plane_1;
extern PxRigidDynamic* player;
extern Player* vehicle;

extern Shader* envShader;
extern Shader* pointParticleShader;
extern Shader* cloudShader;
extern Shader* flameShader;
extern Shader* smokeShader;
extern Shader* boomFlameShader;

bool autoshooting = true;//射击机制
clock_t last = 0;


void processOtherControlEvents() {
	mouseClick();
}

void keyPress() {
	if (game.state == GAME_STATE::STARTED) {
		Plane_1->ProcessKeyPress();
	}
}


void mouseMove() {

}

void mouseSingleClick(int button, int action) { //鼠标单次点击
	if (game.state == GAME_STATE::STARTED) {
		Plane_1->ProcessMouseClick(button, action);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) { //左键按下
		//UI::CenterText::show(string(HETP_TEXT), 6, 4, true);
		//一直闪动
		//	reinterpret_cast<UI::BorderMaskUI*>(UI::UIManager::getUI(UI::UIID::BORDER_MASK))->show();
	//	reinterpret_cast<UI::ReticleUI*>(UI::UIManager::getUI(UI::UIID::RETICLE))->enableTrack(true);
		//reinterpret_cast<UI::ReticleUI*>(UI::UIManager::getUI(UI::UIID::RETICLE))->updateTargetPosition(glm::vec3(1.f, 1.f, 1.f));
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) { //左键抬起
		//reinterpret_cast<UI::ReticleUI*>(UI::UIManager::getUI(UI::UIID::RETICLE))->enableTrack(false);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) { //右键按下
		//reinterpret_cast<UI::BorderMaskUI*>(UI::UIManager::getUI(UI::UIID::BORDER_MASK))->close();
		/*BoomFlameParticleCluster* flame_cluster = new BoomFlameParticleCluster(5, 10.f,  5.f, glm::vec3(0.f,0.f,0.f), std::vector<string>(), boomFlameShader);
		renderParticleClusterList.push_back(flame_cluster);*/
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) { //右键抬起
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) { //中键按下
		//闪动一次
		//reinterpret_cast<UI::BorderMaskUI*>(UI::UIManager::getUI(UI::UIID::BORDER_MASK))->show(1);
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) { //中键抬起

	}
}

void mouseClick() {
	Plane_1->ProcessMouseClick();
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
	if (mouseButtonPressState[GLFW_MOUSE_BUTTON_RIGHT]) { //鼠标右键
		//glm::vec3 initPos = camera.getPosition() + camera.getFront() * 2.f + camera.getUp() * 0.3f;
		glm::vec3 initPos(131.f, 7.f, 56.f);
		//SmokeParticleCluster(int cloudDensity, float cloudRadis, glm::vec3 initPos, vector<unsigned int> textures, Shader* shader);
		//camera.getPosition() + camera.getFront() * 1.f + camera.getUp() * 0.5f,
		//////// 1、烟雾
		//SmokeParticleCluster* smoke_cluster = new SmokeParticleCluster(
		//	100, 2.f,  // 烟雾密度、烟雾团的半径
		//	90, 0.01f, 3.4f, // 每个烟雾粒子大小、烟雾在y方向的速度、烟雾在y方向上最大能上升的距离
		//	glm::vec3(3.f, 1.5f, 3.f), //初始位置
		//	//camera.getPosition() + camera.getFront() * 1.f,
		//	std::vector<string>(), // 纹理路径列表
		//	smokeShader //渲染此烟雾的shader
		//);
		//renderParticleClusterList.push_back(smoke_cluster);

		///////2、爆炸粒子
		//vector<string> paths;
		//for (int i = 1; i <= 18; i++) {
		//	paths.push_back("model/particle/crash/" + to_string(i) + ".obj"); //机械残骸碎片
		//}

		//createPointParticles(
		//	10, false,
		//	new DebrisParticle(glm::vec3(0.01f, 0.01f, 0.01f), paths, glm::vec3(1.f, 1.f, 0.f), envShader),
		//	glmVec3ToPxVec3(initPos),
		//	false, 2.0, // true是散开
		//	true, 20.0, // true是随机速度
		//	12, 5, // 消失时间、开始渐隐时间
		//	PxVec3(10.f, 5.f, 0.f), //初始速度
		//	PxVec3(2.f, 5.f, 0.f)  //力场
		//);

		///////3、拖尾云	
		///////CloudParticleCluster(int cloudDensity, float cloudRadis, float cloudVy, float cloudMaxY, glm::vec3 initPos, vector<string> textures, Shader* shader)
		//CloudParticleCluster* cloud_cluster = new CloudParticleCluster(
		//1,
		//	100, 0.05f,  //云密度、云团的半径
		//	0.01f, 3.4f, // 云在y方向的速度、云在y方向上最大能上升的距离
		//	glm::vec3(3.f, 1.5f, 3.f), //初始位置
		//	glm::vec3(0.1f, 0.1f, 0.1f), //每片云粒子的缩放
		//	//camera.getPosition() + camera.getFront() * 1.f,
		//	std::vector<string>(), // 纹理路径列表
		//	cloudShader //渲染此烟雾的shader
		//);
		//renderParticleClusterList.push_back(cloud_cluster);


		///////4.flame
		/*FlameParticleCluster* flame_cluster = new FlameParticleCluster(5, 1.f, 5.1f, 10, initPos, std::vector<string>(), flameShader);
		renderParticleClusterList.push_back(flame_cluster);*/

		//std::vector<std::string> ts;
		////ts.push_back("images/textures/smoke/smoke-white-1.png");
		//for (int i = 1; i <= 4; i++)
		//	ts.push_back("images/textures/smoke/smoke-gray-" + std::to_string(i) + ".png");
		//	createSmokeParticles(
		//		50, false,
		//		new SmokeParticle(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.f, 1.f, 0.f), cloudShader, ts),
		//		glmVec3ToPxVec3(camera.getPosition()) + glmVec3ToPxVec3(camera.getFront() * 3.f) + glmVec3ToPxVec3(camera.getUp() * 0.5f), // init pos
		//		true, 0.05, // true是散开, 后面的是散开半径
		//		false, 20.0, // true是随机速度，后面的是最大随机速度
		//		20, 5, // 20s 后粒子系统销毁， 从 5s 开始渐隐
		//		PxVec3(0.f, 0.f, 0.f), //初始速度
		//		PxVec3(0.f, 10.f, 0.f)  //力场
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


