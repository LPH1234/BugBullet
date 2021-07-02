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
Game game(GAME_STATE::INIT, 1920 / 2, 1080 / 2, 1920 / 4.0f, 1080 / 4.0f, 0.f, 0.f, true);
// camera
Camera camera(VIEW_TYPE::THIRD_PERSON, glm::vec3(0.0f, 5.0f, 0.0f));

//light
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
//model position
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

bool gameStarted = true;

void exitCallback(void)
{
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
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GLFWimage icons[1];
	icons[0].pixels = SOIL_load_image(ICON_PATH.c_str(), &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
	glfwSetWindowIcon(window, 1, icons);
	SOIL_free_image_data(icons[0].pixels);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE);

	skyBoxShader = new Shader("shaders/skyboxShader/skybox.vs", "shaders/skyboxShader/skybox.fs");
	envShader = new Shader("shaders/envShader/env.vs", "shaders/envShader/env.fs");
	pointParticleShader = new Shader("shaders/debrisShader/debris.vs", "shaders/debrisShader/debris.fs");
	smokeShader = new Shader("shaders/smokeShader/smoke.vs", "shaders/smokeShader/smoke.fs");
	flameShader = new Shader("shaders/flameShader/flame.vs", "shaders/flameShader/flame.fs");
	cloudShader = new Shader("shaders/cloudShader/cloud.vs", "shaders/cloudShader/cloud.fs");

	atexit(exitCallback); //6
	initPhysics(true); //6

	// var init
	// -----------------------------
	for (int i = 0; i <= 348; i++) {
		keyToPressState[i] = false;
		keyToPrePressState[i] = false;
	}
	mouseButtonPressState[0] = false;
	mouseButtonPressState[1] = false;
	mouseButtonPressState[2] = false;

	// build and compile shaders
	// -------------------------
	std::vector<string> faces;
	TextureManager::getSkyBoxTextures(faces);
	skybox = new SkyBox(camera.getPosition(), glm::vec3(1000.f), "", skyBoxShader, faces);
	faces.clear();

	/*FlameParticleCluster* flame_cluster = new FlameParticleCluster(5, 1.f, 5.1f, 10, glm::vec3(0.1f), std::vector<string>(), flameShader);
	renderParticleClusterList.push_back(flame_cluster);*/

	ModelManager::init();
	UIManager::init(game.SCR_WIDTH, game.SCR_HEIGHT);
	MainMenu::init(window);


	game.state = GAME_STATE::MAIN_MENU;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		game.deltaTime = currentFrame - game.lastFrame;
		game.lastFrame = currentFrame;
		// ��Ⱦ
			//---------------------------
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (game.state == GAME_STATE::STARTED) {

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
			// ���ư�Χ��
			//glDepthFunc(GL_LEQUAL); // ��Ȳ������� С�ڵ���
			skyBoxShader->use();
			skyBoxShader->setMat4("projection", projection);
			skyBoxShader->setMat4("view", view);
			skybox->setPosition(glm::vec3(0.f, 0.f, 0.f));
			skyBoxShader->setMat4("model", skybox->getModel());
			skybox->draw();

			//=====================================envShader=================================
			// һ���������Ⱦ������envShader����
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
			envShader->setVec3("light.diffuse", 0.6f, 0.6f, 0.6f); // �����յ�����һЩ�Դ��䳡��
			envShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);


			Render::renderActors(envShader);// ��Ⱦ�����ڵ�����

			Render::renderParticles(physicsParticleSystemList, renderParticleClusterList, view, projection); // ��Ⱦ�����ڵ�����

			Render::renderUI(game.SCR_WIDTH, game.SCR_HEIGHT); //��ȾUI����
		}
		else if (game.state == GAME_STATE::PAUSE) {
			MainMenu::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
		}
		else if (game.state == GAME_STATE::INIT) {
			MainMenu::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
		}
		else {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			if (game.state == GAME_STATE::MAIN_MENU) {
				MainMenu::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
				CornerTip::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
				if (HelpModal::visable)//��¼
					HelpModal::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
				if (ConfigModal::visable)//����
					ConfigModal::draw(game.SCR_WIDTH, game.SCR_HEIGHT);
			}
			else if (game.state == GAME_STATE::OVER) {
			}
			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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


bool last_key = 0;
//����ʱ�����ڵĴ����߼�
void windowProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && last_key) {
		//
		CookThread::shutdown();
		game.state = GAME_STATE::MAIN_MENU;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	last_key = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

//����ʱ������Ĵ����߼�
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
	cameraProcessInput(window);

	keyPress();

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

	camera.ProcessMouseMovement(xoffset, yoffset);
	mouseMove();
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	mouseButtonPressState[button] = action == GLFW_PRESS;
	// button: GLFW_MOUSE_BUTTON_LEFT\GLFW_MOUSE_BUTTON_MIDDLE\GLFW_MOUSE_BUTTON_RIGHT
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
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



