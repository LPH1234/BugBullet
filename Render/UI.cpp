#include "UI.h"

extern Game game;

GLFWwindow* MainMenu::window = nullptr;
bool MainMenu::visable = false;

bool CornerTip::visable = false;

bool ConfigModal::visable = false;
int ConfigModal::soundEffect = 0;
int ConfigModal::soundBg = 0;

bool RecordModal::visable = false;
unordered_map<UIID, BaseUI*> UIManager::idToUI;

void UIManager::addUI(BaseUI* ui) {
	if (idToUI.find(ui->getUIID()) == idToUI.end()) {
		idToUI[ui->getUIID()] = ui;
	}
}


BaseUI* UIManager::getUI(UIID id) {
	if (idToUI.find(id) != idToUI.end()) {
		return idToUI[id];
	}
	return nullptr;
}
void UIManager::setUIVisable(UIID id, bool v) {
	if (idToUI.find(id) != idToUI.end())
		idToUI[id]->setVisable(v);
}
bool UIManager::getUIVisable(UIID id) {
	if (idToUI.find(id) != idToUI.end())
		return	idToUI[id]->getVisable();
	return false;
}

void  UIManager::draw(const float W, const float H) {
	for (unordered_map<UIID, BaseUI*>::iterator iter = idToUI.begin(); iter != idToUI.end(); iter++) {
		BaseUI* ui = (*iter).second;
		if (ui->getVisable()) {
			ui->draw(W,H);
		}
	}
}

void  UIManager::init(const float W, const float H) {
	////ÑªÌõ
	HPBarUI* HPBar;
	HPBar = new HPBarUI(UIID::HP_BAR , H, "images/textures/hpbar-border.png");
	addUI(HPBar);

}

BaseUI::BaseUI(UIID id) {
	this->id = id;
}


bool BaseUI::getVisable() {
	return this->visable;
}
void BaseUI::setVisable(bool v) {
		this->visable = v;
}
UIID BaseUI::getUIID() {
	return this->id;
}
void BaseUI::setUIID(UIID id) {
	this->id = id;
}

glm::vec2 BaseUI::getPos() {
	return this->position;
}
void BaseUI::setPos(glm::vec2 pos) {
	this->position = pos;
}
glm::vec2 BaseUI::getSize() {
	return this->size;
}
void BaseUI::setSize(glm::vec2 size) {
	this->size = size;
}


HPBarUI::HPBarUI(UIID id, float H, std::string texture):BaseUI(id)
{
	position = glm::vec2(10, H - 18 - 10);
	size = glm::vec2(170.f, 18.f);
	HPBarFillShader = new Shader("shaders/HPBarShader/HPBar.vs", "shaders/HPBarShader/HPBarFill.fs");
	HPBarBorderShader = new Shader("shaders/HPBarShader/HPBar.vs", "shaders/HPBarShader/HPBarBorder.fs");

	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	loadTexture(texture.c_str(), &textureID);
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

HPBarUI::~HPBarUI()
{
	glDeleteVertexArrays(1, &this->VAO);
}

void HPBarUI::draw(unsigned int w, unsigned int h)
{
	glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	const float FILL_WIDTH = 148.f;
	const float FILL_HEIGHT = 14.f;
	glDepthFunc(GL_ALWAYS);
	if (enableAnimate) {
		if (this->animateProgress < this->progress)
			this->animateProgress += animateVelocity;
		else if (this->animateProgress > this->progress)
			this->animateProgress -= animateVelocity;
	}
	else {
		this->animateProgress = this->progress;
	}
	
	/*1£ºhpÌî³ä*/
	this->HPBarFillShader->use();
	glm::vec3 position0(position.x + (size.x - FILL_WIDTH) / 2.f, position.y + (size.y - FILL_HEIGHT) / 2.f, 0.f);
	std::cout << "border pos:" << position.x << "   " << position.y << "\n";
	std::cout << "fill pos:" << position0.x << "   " << position0.y << "\n";
	glm::vec2 size0(FILL_WIDTH, FILL_HEIGHT);
	model = glm::mat4(1.0f);
	model = glm::translate(model, position0);  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::scale(model, glm::vec3(FILL_WIDTH, FILL_HEIGHT, 1.0f)); // border 170 18
	this->length = size0.x;
	this->HPBarFillShader->setMat4("model", model);
	this->HPBarFillShader->setMat4("projection", projection);
	this->HPBarFillShader->setFloat("progress", this->animateProgress);
	this->HPBarFillShader->setFloat("length", this->length);
	this->HPBarFillShader->setFloat("leftTopX", position0.x);
	/*this->HPBarFillShader->setFloat("leftTopY;", position0.y);
	this->HPBarFillShader->setFloat("rightBottomX;", position0.x + size0.x);
	this->HPBarFillShader->setFloat("rightBottomY;", position0.y + size0.y);*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*2£ºhp¿ò*/
	this->HPBarBorderShader->use();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
	this->HPBarBorderShader->setMat4("model", model);
	this->HPBarBorderShader->setMat4("projection", projection);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

}

void HPBarUI::updateProgress(float progress) {
	this->progress = progress;
}

void HPBarUI::setEnableAnimate(bool enbale) { enableAnimate = enbale; }






void MainMenu::init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	MainMenu::window = window;
}

void MainMenu::draw(unsigned int w, unsigned int h) {
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	// ------------------------Start the Dear ImGui frame

	{
		static float f = 0.0f;
		static int counter = 0;
		ImVec2 window_size(360, 240);
		ImGui::Begin("hp", NULL, window_flags);                          // Create a window called "Hello, world!" and append into it.
		ImGui::SetNextWindowBgAlpha(0.f);
		ImGui::SetWindowSize(window_size);
		if (ImGui::Button("S t a r t", ImVec2(window_size.x, window_size.y / 4))) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
			game.state = GAME_STATE::STARTED;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (ImGui::Button("R e c o r d", ImVec2(window_size.x, window_size.y / 4))) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
			RecordModal::visable = true;
		}
		if (ImGui::Button("C o n f i g", ImVec2(window_size.x, window_size.y / 4))) {   
			ConfigModal::visable = true;
		}
		if (ImGui::Button("E x i t", ImVec2(window_size.x, window_size.y / 4))) {   
			glfwSetWindowShouldClose(window, true);
		}

		ImGui::SetWindowPos(ImVec2(w / 2, h / 2));
		ImGui::End();
	}
	

}



 void CornerTip::init(GLFWwindow* window) {

}
 void CornerTip::draw(unsigned int w, unsigned int h) {
	bool* p_open = NULL;
	const float DISTANCE = 10.0f;
	static int corner = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (corner != -1)
	{
		window_flags |= ImGuiWindowFlags_NoMove;
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
	{
		ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}


 void ConfigModal::init(GLFWwindow* window) {

 }

 void ConfigModal::draw(unsigned int w, unsigned int h) {
	 ImGui::OpenPopup("Config");
	 if (ImGui::BeginPopupModal("Config", NULL, ImGuiWindowFlags_MenuBar))
	 {
		 // Testing behavior of widgets stacking their own regular popups over the modal.
		 static int preSoundEffect = soundEffect;
		 ImGui::Combo("Sound", &soundEffect, "Enable\0Disable\0\0"); 
		 if (preSoundEffect != soundEffect) ConfigModal::configSoundEffect();

		 static int preSoundBg = soundBg;
		 ImGui::Combo("Sound", &soundBg, "Enable\0Disable\0\0");
		 if (preSoundBg != soundBg) ConfigModal::configSoundBg();

		 if (ImGui::Button("Add another modal.."))
			 ImGui::OpenPopup("Stacked 2");

		 // Also demonstrate passing a bool* to BeginPopupModal(), this will create a regular close button which
		 // will close the popup. Note that the visibility state of popups is owned by imgui, so the input value
		 // of the bool actually doesn't matter here.
		 bool unused_open = true;
		 if (ImGui::BeginPopupModal("Stacked 2", &unused_open))
		 {
			 ImGui::Text("Hello from Stacked The Second!");
			 if (ImGui::Button("Close"))
				 ImGui::CloseCurrentPopup();
			 ImGui::EndPopup();
		 }

		 ImVec2 closeBtnSize(w / 3.f, h / 10.f);
		 if (ImGui::Button("Close", closeBtnSize)) {
			 ImGui::CloseCurrentPopup();
			 ConfigModal::visable = false;
		 }
		 ImGui::EndPopup();
	 }
 }

void ConfigModal::configSoundEffect() {

}
void ConfigModal::configSoundBg() {

}

 void RecordModal::init(GLFWwindow* window) {

 }
 void RecordModal::draw(unsigned int w, unsigned int h) {

 }