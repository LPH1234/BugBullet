#pragma execution_character_set("utf-8")

#ifdef RENDER_SNIPPET

#include "Controller.h"
#include "../Render/UI.h"

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);
extern void cleanupPhysics(bool interactive);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void updateKeyState(GLFWwindow* window, std::unordered_map<int, bool>& map);
// settings
Game game(GAME_STATE::INIT,MEDIA_STATE::init, 1920 / 2, 1080 / 2, 1920 / 4.0f, 1080 / 4.0f, 0.f, 0.f, true);

// camera
Camera camera(VIEW_TYPE::THIRD_PERSON, glm::vec3(0.0f, 5.0f, 0.0f));

//light
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 32.0f, 0.0f);


SkyBox* skybox;

Shader* skyBoxShader;
Shader* envShader;
Shader* pointParticleShader;
Shader* cloudShader;
Shader* flameShader;
Shader* smokeShader;

std::unordered_map<int, bool> keyToPressState;
std::unordered_map<int, bool> keyToPrePressState;
bool mouseButtonPressState[3];
extern Media MediaPlayer;									//声音播放器

void exitCallback(void) {
	cleanupPhysics(true);
}

int myRenderLoop()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	// glfw window creation
	// -------------------- 
	GLFWwindow* window = glfwCreateWindow(game.SCR_WIDTH, game.SCR_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
	if (window == NULL) {
		Logger::error("Failed to create GLFW window!");
		glfwTerminate();
		return -1;
	}
	UI::initIcon(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		Logger::error("Failed to initialize GLAD!");
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE);

	TextureManager::init();
	TextureManager::initAnimateTextures(); // 开启动画，执行此函数会开始加载动画帧。
	UI::UIManager::init(window, game.SCR_WIDTH, game.SCR_HEIGHT);
	game.state = GAME_STATE::INIT; // 将游戏的初始状态设置为INIT状态，游戏状态是一个有限状态机
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		game.deltaTime = currentFrame - game.lastFrame;
		game.lastFrame = currentFrame;
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (game.state == GAME_STATE::STARTED || game.state == GAME_STATE::PAUSE || game.state == GAME_STATE::OVER) { // 游戏画面及其附加状态

			// input
			// -----
			processInput(window);

			// self define actions processor
			// -----
			processOtherControlEvents();

			// PhysX Simulation
			// -------------------------------------------------------------------------------
			stepPhysics(!game.pause);

			// GL Render Process
			// -------------------------------------------------------------------------------
			glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), game.SCR_WIDTH / game.SCR_HEIGHT, 1.f, 12500.f);
			camera.trackDynamicPosition();
			glm::mat4 view = camera.GetViewMatrix();

			//=====================================skyBoxShader=================================
			// 绘制包围盒
			//glDepthFunc(GL_LEQUAL); // 深度测试条件 小于等于
			skyBoxShader->use();
			skyBoxShader->setMat4("projection", projection);
			skyBoxShader->setMat4("view", view);
			skybox->setPosition(glm::vec3(0.f, 0.f, 0.f));
			skyBoxShader->setMat4("model", skybox->getModel());
			skybox->draw();

			//=====================================envShader=================================
			// 一般物体的渲染表现由envShader决定
			envShader->use();
			envShader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
			envShader->setMat4("projection", projection);
			envShader->setMat4("view", view);
			envShader->setVec3("viewPos", camera.getPosition());
			envShader->setInt("material.diffuse", 0);
			envShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
			envShader->setFloat("material.shininess", 32.0f);
			envShader->setVec3("light.position", lightPosition);
			envShader->setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
			envShader->setVec3("light.diffuse", 0.6f, 0.6f, 0.6f); // 将光照调暗了一些以搭配场景
			envShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			Render::renderActors(envShader);// 渲染场景内的物体

			Render::renderParticles(physicsParticleSystemList, renderParticleClusterList, view, projection); // 渲染场景内的粒子

			UI::UIManager::setUIVisable(UI::UIID::HP_BAR, true);
			UI::UIManager::setUIVisable(UI::UIID::MAIN_ANIMATION, false);
			UI::UIManager::setUIVisable(UI::UIID::RETICLE, mouseButtonPressState[1]); //按下右键出现准心
			Render::renderUI(game.SCR_WIDTH, game.SCR_HEIGHT); //渲染UI界面

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			UI::CornerTip::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::PlayerStatus::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::PauseMenu::visable = game.state == GAME_STATE::PAUSE;
			UI::PauseMenu::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::OverModal::visable = game.state == GAME_STATE::OVER;
			UI::OverModal::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::CenterText::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			ImGui::Render();// 渲染ImgUI界面
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			

		}
		else if (game.state == GAME_STATE::INIT) { // 初始状态，初始化及渲染初始帧
			UI::UIManager::setUIVisable(UI::UIID::HP_BAR, false);
			UI::UIManager::setEnableAnimate(UI::UIID::MAIN_ANIMATION, false);
			UI::UIManager::setUIVisable(UI::UIID::MAIN_ANIMATION, true); // 以上两行为了渲染动画的默认帧
			Render::renderUI(game.SCR_WIDTH, game.SCR_HEIGHT); //渲染UI界面
			if (!game.isInit) { //如果没有进行过初始化
				atexit(exitCallback);
				// build and compile shaders
				// -------------------------
				skyBoxShader = new Shader("shaders/skyboxShader/skybox.vs", "shaders/skyboxShader/skybox.fs");
				envShader = new Shader("shaders/envShader/env.vs", "shaders/envShader/env.fs");
				pointParticleShader = new Shader("shaders/debrisShader/debris.vs", "shaders/debrisShader/debris.fs");
				smokeShader = new Shader("shaders/smokeShader/smoke.vs", "shaders/smokeShader/smoke.fs");
				flameShader = new Shader("shaders/flameShader/flame.vs", "shaders/flameShader/flame.fs");
				cloudShader = new Shader("shaders/cloudShader/cloud.vs", "shaders/cloudShader/cloud.fs");
				ModelManager::init();
				UI::initImgUI(window);
				UI::MainMenu::init(window);
				UI::ConfigModal::init(window);
				UI::HelpModal::init(window);
				UI::CornerTip::init(window, &camera);
				UI::PlayerStatus::init(window);
				UI::PauseMenu::init(window);
				UI::CenterText::init(window);
				UI::OverModal::init(window);

				// var init
				// -----------------------------
				for (int i = 0; i <= 348; i++)
					keyToPrePressState[i] = keyToPressState[i] = false;
				for (int i = 0; i < 3; i++)
					mouseButtonPressState[i] = false;
				std::vector<string> faces;
				TextureManager::getSkyBoxTextures(faces);
				skybox = new SkyBox(camera.getPosition(), glm::vec3(1000.f), "", skyBoxShader, faces);
				faces.clear();
				initPhysics(true);
				game.state = GAME_STATE::MAIN_MENU;
				game.isInit = true;
				MediaPlayer.PlayBackground(Media::MediaType::START);
			}
		}
		else if (game.state == GAME_STATE::MAIN_MENU) { // 主菜单界面
			// resetPhysicsObjState();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			UI::UIManager::setEnableAnimate(UI::UIID::MAIN_ANIMATION, TextureManager::getAnimationLoadProgress() == 1);
			UI::UIManager::setUIVisable(UI::UIID::MAIN_ANIMATION, true);
			//	std::cout << "tex loadTexProgress:" << TextureManager::loadTexProgress << "   " << TextureManager::getAnimationLoadProgress() << "\n";
			UI::UIManager::setUIVisable(UI::UIID::HP_BAR, false);
			UI::MainMenu::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::HelpModal::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			UI::ConfigModal::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			Render::renderUI(game.SCR_WIDTH, game.SCR_HEIGHT); //渲染UI界面
			ImGui::Render();// 渲染ImgUI界面
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			//MediaPlayer.PlayBackground(Media::MediaType::START);
			
			if (game.deltaTime < 0.016f)
				Sleep((int)((0.016f - game.deltaTime) * 1000)); //最高每秒60帧
		}


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	delete envShader;
	delete skyBoxShader;

	return 0;

}

//按键时，窗口的处理逻辑
void windowProcessInput(GLFWwindow *window) {
	if (keyToPressState[GLFW_KEY_ESCAPE] && !keyToPrePressState[GLFW_KEY_ESCAPE]) {
		if (game.state == GAME_STATE::STARTED) { //请求暂停
			game.state = GAME_STATE::PAUSE;
			game.pause = true;
			UI::UIManager::setCursorVisable(true);

		}
		else if (game.state == GAME_STATE::PAUSE) { //继续游戏
			game.state = GAME_STATE::STARTED;
			game.pause = false;
			UI::UIManager::setCursorVisable(false);
		}

	}
}

//按键时，相机的处理逻辑
void cameraProcessInput(GLFWwindow *window) {
	if (keyToPressState[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, game.deltaTime);
	if (keyToPressState[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, game.deltaTime);
	if (keyToPressState[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, game.deltaTime);
	if (keyToPressState[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, game.deltaTime);
	if (keyToPressState[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(UP, game.deltaTime);
	if (keyToPressState[GLFW_KEY_LEFT_CONTROL])
		camera.ProcessKeyboard(DOWN, game.deltaTime);
	if (keyToPressState[GLFW_KEY_LEFT_SHIFT])
		camera.ProcessKeyboard(SHIFT_PRESS, game.deltaTime);
	if (!keyToPressState[GLFW_KEY_LEFT_SHIFT])
		camera.ProcessKeyboard(SHIFT_RELEASE, game.deltaTime);
	if (keyToPressState[GLFW_KEY_F1])
		camera.setMode(VIEW_TYPE::FIRST_PERSON);
	if (keyToPressState[GLFW_KEY_F3])
		camera.setMode(VIEW_TYPE::THIRD_PERSON);
	if (keyToPressState[GLFW_KEY_F2])
		camera.setMode(VIEW_TYPE::FREE);
	if (keyToPressState[GLFW_KEY_F4])
		camera.setMode(VIEW_TYPE::BEHIND_PERSON_TRACK_ALL_DIRECTION);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	updateKeyState(window, keyToPressState);
	windowProcessInput(window);
	if (game.state == GAME_STATE::STARTED) {
		cameraProcessInput(window);
		keyPress();
	}
	updateKeyState(window, keyToPrePressState);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	game.SCR_WIDTH = width;
	game.SCR_HEIGHT = height;
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (game.firstMouse)
	{
		game.lastX = xpos;
		game.lastY = ypos;
		game.firstMouse = false;
	}

	float xoffset = xpos - game.lastX;
	float yoffset = game.lastY - ypos; // reversed since y-coordinates go from bottom to top

	game.lastX = xpos;
	game.lastY = ypos;
	if (game.state == GAME_STATE::STARTED) {
		camera.ProcessMouseMovement(xoffset, yoffset);
		mouseMove();
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	mouseButtonPressState[button] = action == GLFW_PRESS;
	mouseSingleClick(button, action);
	// button: GLFW_MOUSE_BUTTON_LEFT\GLFW_MOUSE_BUTTON_MIDDLE\GLFW_MOUSE_BUTTON_RIGHT
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (game.state == GAME_STATE::STARTED)
		camera.ProcessMouseScroll(yoffset);
}


void updateKeyState(GLFWwindow* window, std::unordered_map<int, bool>& map)
{
	const int STATE = GLFW_PRESS;
	map[96] = glfwGetKey(window, 96) == STATE;
	map[32] = glfwGetKey(window, 32) == STATE;

	for (int i = 44; i <= 61; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 65; i <= 93; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 44; i <= 61; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 256; i <= 269; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 280; i <= 314; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 320; i <= 336; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
	for (int i = 340; i <= 348; i++)
	{
		map[i] = glfwGetKey(window, i) == STATE;
	}
}


#endif



