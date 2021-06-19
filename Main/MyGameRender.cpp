#pragma execution_character_set("utf-8")

#ifdef RENDER_SNIPPET


#include "Controller.h"



using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);
extern void cleanupPhysics(bool interactive);

extern PxRigidDynamic* player_ctl;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void updateKeyState(GLFWwindow* window, std::unordered_map<int, bool>& map, const int STATE);
// settings
const unsigned int SCR_WIDTH = 1920 / 2;
const unsigned int SCR_HEIGHT = 1080 / 2;

// camera
Camera camera(VIEW_TYPE::THIRD_PERSON, glm::vec3(0.0f, 5.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//light
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
//model position
glm::vec3 lightPosition = glm::vec3(0.0f, 32.0f, 0.0f);


SkyBox* skybox;
Shader* skyBoxShader;
Shader* envShader;

unordered_map<int, bool>keyboard_input(false);//处理键盘输入


//
std::unordered_map<int, bool> keyToPressState;
std::unordered_map<int, bool> keyToPrePressState;



void renderCallback(Shader* shader)
{
	stepPhysics(true);
	PxScene* scene;
	PxGetPhysics().getScenes(&scene, 1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		Snippets::renderActors(&actors[0], static_cast<PxU32>(actors.size()), shader, true);
	}
}

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	for (int i = 0; i <= 348; i++)
		keyToPressState[i] = false;
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

	// build and compile shaders
	// -------------------------
	skyBoxShader = new Shader("shaders/skyboxShader/skybox.VertexShader", "shaders/skyboxShader/skybox.FragmentShader");
	envShader = new Shader("shaders/envShader/env.VertexShader", "shaders/envShader/env.FragmentShader");


	atexit(exitCallback); //6
	initPhysics(true); //6



	skybox = new SkyBox(camera.getPosition(), glm::vec3(7000.0f, 7000.0f, 7000.0f), "", skyBoxShader);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		envShader->use();
		envShader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.f, 12000.f);
		camera.trackDynamicPosition();
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
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


		envShader->setMat4("projection", projection);
		envShader->setMat4("view", view);

		renderCallback(envShader);
		//=====================================skyBoxShader=================================
		// 绘制包围盒
		//glDepthFunc(GL_LEQUAL); // 深度测试条件 小于等于

		skyBoxShader->use();
		skyBoxShader->setMat4("projection", projection);
		skyBoxShader->setMat4("view", view);


		skybox->setPosition(camera.getPosition());
		skyBoxShader->setMat4("model", skybox->getModel());
		skybox->draw();

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
//按键时，窗口的处理逻辑
void windowProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE && last_key) {
		glfwSetWindowShouldClose(window, true);
		CookThread::shutdown();
	}
	last_key = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

//按键时，相机的处理逻辑
void cameraProcessInput(GLFWwindow *window) {
	if (keyToPressState[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keyToPressState[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keyToPressState[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keyToPressState[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keyToPressState[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keyToPressState[GLFW_KEY_LEFT_CONTROL])
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (keyToPressState[GLFW_KEY_LEFT_SHIFT])
		camera.ProcessKeyboard(SHIFT_PRESS, deltaTime);
	if (!keyToPressState[GLFW_KEY_LEFT_SHIFT])
		camera.ProcessKeyboard(SHIFT_RELEASE, deltaTime);
	if (keyToPressState[GLFW_KEY_F1])
		camera.setMode(VIEW_TYPE::FIRST_PERSON);
	if (keyToPressState[GLFW_KEY_F3])
		camera.setMode(VIEW_TYPE::THIRD_PERSON);
	if (keyToPressState[GLFW_KEY_F2])
		camera.setMode(VIEW_TYPE::FREE);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	updateKeyState(window, keyToPressState, GLFW_PRESS);
	windowProcessInput(window);
	cameraProcessInput(window);

	keyPress();

	updateKeyState(window, keyToPrePressState, GLFW_PRESS);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
	mouseMove();
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void updateKeyState(GLFWwindow* window, std::unordered_map<int, bool>& map, const int STATE)
{
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



