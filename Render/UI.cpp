#include "UI.h"
extern void resetLevel();
extern Game game;
extern Media MediaPlayer;
namespace UI {
	ImFont* zhFont = nullptr;
	ImFont* logoFont = nullptr;
	ImFont* titleFont = nullptr;
	ImFont* defaultFont = nullptr;

	unordered_map<UIID, BaseUI*> UIManager::idToUI;
	GLFWwindow* UIManager::window;
	glm::mat4 UIManager::projection;
	glm::mat4 UIManager::view;

	GLFWwindow* MainMenu::window = nullptr;
	bool MainMenu::visable = false;

	bool PauseMenu::visable = false;
	GLFWwindow* PauseMenu::window = nullptr;


	bool ConfigModal::visable = false;
	int ConfigModal::soundEffect = 0;
	int ConfigModal::soundBg = 0;
	int ConfigModal::preSoundEffect = 0; ;
	int ConfigModal::preSoundBg = 0;
	ImGuiStyle*  ConfigModal::style = nullptr;

	bool HelpModal::visable = false;

	bool CornerTip::visable = false;
	Camera* CornerTip::camera = nullptr;

	bool PlayerStatus::visable = false;
	int PlayerStatus::HP = 100;
	int PlayerStatus::Ammo = 26;
	int PlayerStatus::Missile = 5;

	std::string CenterText::text = "";
	bool CenterText::visable = false;
	unsigned int CenterText::fadeTime = 0U; //开始消失的时间：ms
	unsigned int CenterText::timeToLeave = 0U;//完全消失的时间：ms
	unsigned int CenterText::showTime = 0U;//开始显示的时间：ms
	float CenterText::currAlpha = 1.f;
	bool CenterText::isBling = false;
	int  CenterText::blingTimes = 0;
	float  CenterText::blingValue = 0.f;
	bool CenterText::blingDown = false;

	std::string LogoText::text;
	bool LogoText::visable = false;


	bool OverModal::visable = false;
	bool OverModal::isFail = true;

	int MissionModal::maxLevel;
	int MissionModal::currLevel;
	bool MissionModal::visable;
	unordered_map<int, std::string> MissionModal::levelToTaskInfo;
	unordered_map<int, std::string> MissionModal::levelToTaskIcon;
	int** MissionModal::currBeatAndTotal = nullptr;

	void initIcon(GLFWwindow* window) {
		GLFWimage icons[1];
		icons[0].pixels = SOIL_load_image(ICON_PATH.c_str(), &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
		glfwSetWindowIcon(window, 1, icons);
		SOIL_free_image_data(icons[0].pixels);
	}
	void initImgUI(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		const char* glsl_version = "#version 330";
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		defaultFont = io.Fonts->AddFontFromFileTTF(EN_FONT_PATH.c_str(), EN_FONT_SIZE); // 英文字体(默认字体）
		logoFont = io.Fonts->AddFontFromFileTTF(EN_FONT_PATH.c_str(), EN_LOGO_FONT_SIZE); // 英文logo字体
		titleFont = io.Fonts->AddFontFromFileTTF(EN_TITLE_FONT_PATH.c_str(), EN_TITLE_FONT_SIZE); // 英文标题字体
		zhFont = io.Fonts->AddFontFromFileTTF(ZH_CN_FONT_PATH.c_str(), ZH_CN_FONT_SIZE, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon()); //中文字体
	}
	void backToMain() {
		game.state = GAME_STATE::MAIN_MENU;
		game.pause = false;
		reinterpret_cast<UI::BorderMaskUI*>(UI::UIManager::getUI(UI::UIID::BORDER_MASK))->close(true);

		MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
		MediaPlayer.StopBackground();
		MediaPlayer.PlayBackground(Media::MediaType::START);
		resetLevel();
	}

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

	void UIManager::setEnableAnimate(UIID id, bool enable) {
		if (idToUI.find(id) != idToUI.end())
			idToUI[id]->setEnableAnimate(enable);
	}


	void  UIManager::draw(const float W, const float H) {
		for (unordered_map<UIID, BaseUI*>::iterator iter = idToUI.begin(); iter != idToUI.end(); iter++) {
			BaseUI* ui = (*iter).second;
			if (ui->getVisable()) {
				ui->draw(W, H);
			}
		}
	}

	void  UIManager::init(GLFWwindow* window, const float W, const float H) {
		UIManager::window = window;
		////血条
		HPBarUI* HPBar;
		HPBar = new HPBarUI(UIID::HP_BAR, H, HPBAR_TEXTURE_PATH);
		addUI(HPBar);
		/////动画
		AnimationUI* Animation = new AnimationUI(UIID::MAIN_ANIMATION, W, H, &TextureManager::animateTextureIDs, ANIMATE_START_FRAME);
		addUI(Animation);

		BorderMaskUI* borderMask = new BorderMaskUI(UIID::BORDER_MASK, W, H, TextureManager::getTextureID(BORDER_MASK_UI_TEX_PATH), BORDER_MASK_UI_CLOSE_DELAY);
		addUI(borderMask);

		ReticleUI* reticleUI = new ReticleUI(UIID::RETICLE, W, H, RETICLE_TEXTURE_PATH);
		addUI(reticleUI);
	}

	void UIManager::setCursorVisable(bool v) {
		glfwSetInputMode(UIManager::window, GLFW_CURSOR, v ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	void UIManager::ImgUIBeginFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void UIManager::ImgUIEndFrame() {
		ImGui::Render();// 渲染ImgUI界面
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	void  BaseUI::setEnableAnimate(bool enable) {
		this->enableAnimate = enable;
	}

	HPBarUI::HPBarUI(UIID id, float H, std::string texture) :BaseUI(id)
	{
		position = glm::vec2(XOFFSET, H - YOFFSET);
		size = glm::vec2(WIDTH, HEIGHT);
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
		position.y = h - YOFFSET;
		glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		const float FILL_WIDTH = 148.f;
		const float FILL_HEIGHT = 14.f;
		updateProgress(UI::PlayerStatus::HP);
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
		/*1：hp填充*/
		this->HPBarFillShader->use();
		glm::vec3 position0(position.x + (size.x - FILL_WIDTH) / 2.f, position.y + (size.y - FILL_HEIGHT) / 2.f, 0.f);
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

		/*2：hp框*/
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

	// Constructor (inits shaders/shapes)
	ReticleUI::ReticleUI(UIID id, float W, float H, std::string texture) :BaseUI(id) {
		shader = new Shader("shaders/reticleShader/reticle.vs", "shaders/reticleShader/reticle.fs");
		// Configure VAO/VBO
		size.y = size.x = W / RETICLE_SIZE_RELATIVE_FACTOR;
		initPos.x = position.x = W / 2.f - size.x / 2;
		initPos.y = position.y = H / 2.f - size.y / 2;
		this->visable = false;
		this->currAngle = false;
		this->ableTrack = false;
		this->blingDown = true;
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
		textureID = TextureManager::getTextureID(texture);
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
	// Destructor
	ReticleUI::~ReticleUI() {
		delete this->shader;
	}
	// Renders a defined quad textured with given sprite
	void ReticleUI::draw(unsigned int w, unsigned int h) {
		size.y = size.x = w / RETICLE_SIZE_RELATIVE_FACTOR;
		glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		this->shader->use();
		model = glm::mat4(1.0f);
		if (!this->ableTrack) {//如果没有目标
			initPos.x = position.x = w / 2.f - size.x / 2;
			initPos.y = position.y = h / 2.f - size.y / 2;
			model = glm::translate(model, glm::vec3(initPos.x, initPos.y, 0.1f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
			model = glm::rotate(model, glm::radians(1.f*(currAngle++ % 360)), glm::vec3(0.f, 0.f, 1.f)); //绕轴rorate_axis旋转，如果rorate_angle是数字形式
			blingValue = 1.f;
		}
		else {
			const float dist = glm::distance(position, targetPos);
			const glm::vec2& ori = glm::normalize(targetPos - position);
			if (dist <= RETICLE_BEGIN_TO_BLING_DISTANCE) { //到了闪动的距离
				blingValue += blingDown ? -RETICLE_BLING_VELOCITY : RETICLE_BLING_VELOCITY;
				if (blingValue >= 1.f) {
					blingDown = true; blingValue = 1.f;
				}
				if (blingValue <= 0.f) {
					blingDown = false; blingValue = 0.f;
				}
			}
			if (dist <= RETICLE_LOCK_DISTANCE) { //锁定距离
				position = targetPos; blingValue = 0.f;
			}
			else {
				position += (RETICLE_LOCK_DISTANCE / 1.2f * ori);
			}
			model = glm::translate(model, glm::vec3(position.x, position.y, 0.1f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
		}
		model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
		this->shader->setMat4("model", model);
		this->shader->setMat4("projection", projection);
		this->shader->setFloat("alpha", 1.0);
		this->shader->setFloat("blingValue", blingValue);
		//glDepthFunc(GL_ALWAYS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
	}
	void ReticleUI::updateTargetPosition(glm::vec3& pos) { //     gl_Position = projection * view * model * vec4(aPos, 1.0);
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 temp = glm::project(pos, UIManager::view * model, UIManager::projection, glm::vec4(0.f, 0.f, game.SCR_WIDTH, game.SCR_HEIGHT));
		temp.x = temp.x < size.x ? size.x : temp.x;;
		temp.x = temp.x > game.SCR_WIDTH - size.x ? game.SCR_WIDTH - size.x : temp.x;
		temp.y = temp.y < size.y ? size.y : temp.y;;
		temp.y = temp.y > game.SCR_HEIGHT - size.y ? game.SCR_HEIGHT - size.y : temp.y;
		this->targetPos.x = temp.x;
		this->targetPos.y = game.SCR_HEIGHT - temp.y;
		//std::cout << "x:" << this->targetPos.x << "    y:" << this->targetPos.y << "\n";
	}
	void  ReticleUI::enableTrack(bool enable) {
		this->ableTrack = enable;
	}

	BorderMaskUI::BorderMaskUI(UIID id, float W, float H, unsigned int textureID, float closeDelay) : BaseUI(id) {
		position = glm::vec2(0, 0);
		size = glm::vec2(W, H);
		this->textureID = textureID;
		this->closeDelay = closeDelay * 1000;
		this->visable = false;
		borderMaskShader = new Shader("shaders/borderMaskShader/borderMask.vs", "shaders/borderMaskShader/borderMask.fs");
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
	BorderMaskUI::~BorderMaskUI() {
		delete this->borderMaskShader;
	}
	void BorderMaskUI::draw(unsigned int w, unsigned int h) {
		if (this->shouldClose) {
			if (closeDelay <= 0)
				alpha = 0.f;
			else
				alpha = 1 - (clock() - startCloseTime) * 1.f / closeDelay;
			alpha = alpha < 0.f ? 0.f : alpha;
		}
		else {
			blingValue += blingDown ? -BORDER_MASK_UI_BLING_VELOCITY : BORDER_MASK_UI_BLING_VELOCITY;
			if (blingValue >= 1.f) {
				blingDown = true; blingValue = 1.f;
			}
			if (blingValue <= 0.f) {
				blingDown = false; blingValue = 0.f;
				if (blingTimes > 0 && ++currBlingTimes >= blingTimes) this->close();
			}
		}
		glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
		glm::mat4 model(1.0f);
		size.x = w; size.y = h;
		this->borderMaskShader->use();
		model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
		model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
		this->borderMaskShader->setMat4("model", model);
		this->borderMaskShader->setMat4("projection", projection);
		this->borderMaskShader->setFloat("alpha", alpha);
		this->borderMaskShader->setFloat("blingValue", blingValue);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
		if (alpha == 0.f) {
			UI::UIManager::setUIVisable(this->getUIID(), false);
		}
	}

	void BorderMaskUI::show(int blingTimes) {
		blingValue = 1.f; //当前闪动值，0~1
		blingDown = true; //当前闪动值是否减小方向
		shouldClose = false;
		alpha = 1.f;
		this->blingTimes = blingTimes; //默认参数-1:一直闪动(当blingTimes《=0时一直闪动)
		currBlingTimes = 0;
		UI::UIManager::setUIVisable(this->getUIID(), true);
	}
	void BorderMaskUI::close(bool immediately) {
		if (immediately) UI::UIManager::setUIVisable(this->getUIID(), false);
		if (shouldClose) return;
		shouldClose = true;
		startCloseTime = clock();
	}

	// Constructor (inits shaders/shapes)
	AnimationUI::AnimationUI(UIID id, float W, float H, std::vector<unsigned int>* textureIDs, unsigned int startFrameIndex) : BaseUI(id) {
		position = glm::vec2(0, 0);
		size = glm::vec2(W, H);
		this->textureIDs = textureIDs;
		this->startFrameIndex = startFrameIndex;
		this->currFrameIndex = this->startFrameIndex;
		this->startFramePath = ANIMATE_FRAMS_PATH + to_string(this->startFrameIndex) + ANIMATE_FRAMS_SUFFIX;
		AnimationUIShader = new Shader("shaders/HPBarShader/HPBar.vs", "shaders/HPBarShader/HPBarBorder.fs");
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
	// Destructor
	AnimationUI::~AnimationUI() {
		delete this->AnimationUIShader;
	}
	// Renders a defined quad textured with given sprite
	void AnimationUI::draw(unsigned int w, unsigned int h) {
		glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
		glm::mat4 model(1.0f);
		size.x = w; size.y = h;
		this->AnimationUIShader->use();
		model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
		model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
		this->AnimationUIShader->setMat4("model", model);
		this->AnimationUIShader->setMat4("projection", projection);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		if (enableAnimate) {
			if (currFrameIndex < this->textureIDs->size()) {
				glBindTexture(GL_TEXTURE_2D, (*this->textureIDs)[currFrameIndex++]);
				if (this->currFrameIndex >= this->textureIDs->size()) this->currFrameIndex = 0;
			}
		}
		else {
			glBindTexture(GL_TEXTURE_2D, TextureManager::getTextureID(startFramePath));
		}
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
	}

	void AnimationUI::setStartFrame(unsigned int sf) {
		this->startFrameIndex = sf;
	}




	void MainMenu::init(GLFWwindow* window) {
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
			ImVec2 window_size(360, 360);
			ImGui::Begin("hp", NULL, window_flags);                          // Create a window called "Hello, world!" and append into it.
			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::SetWindowSize(window_size);
			if (ImGui::Button("S t a r t", ImVec2(window_size.x, window_size.y / 5))) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
				game.state = GAME_STATE::STARTED;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
				MediaPlayer.StopBackground();
				//MediaPlayer.PlayBackground(Media::MediaType::FLYING);
				UI::UIManager::setCursorVisable(false);
			}
			if (ImGui::Button("C o n f i g", ImVec2(window_size.x, window_size.y / 5))) {
				ConfigModal::visable = true;
				MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
			}
			if (ImGui::Button("H e l p", ImVec2(window_size.x, window_size.y / 5))) {                           // Buttons return true when clicked (most widgets return true when edited/activated)
				HelpModal::visable = true;
				MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
			}
			if (ImGui::Button("E x i t", ImVec2(window_size.x, window_size.y / 5))) {
				glfwSetWindowShouldClose(window, true);
				MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
				CookThread::shutdown();
			}

			ImGui::SetWindowPos(ImVec2(w * 2 / 3.f, h / 3.f));
			ImGui::End();
		}


	}


	void PauseMenu::init(GLFWwindow* window) {
		PauseMenu::visable = false;
		PauseMenu::window = window;
	}

	void PauseMenu::draw(unsigned int w, unsigned int h) {
		if (PauseMenu::visable) {
			ImGui::OpenPopup("P A U S E");
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.00f, 0.00f, 0.00f, 0.125f)); //模态框背景
			ImGui::SetNextWindowPos(ImVec2(w / 2 - w / 8, h / 2 - h / 6));
			if (ImGui::BeginPopupModal("P A U S E", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
			{
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(PAUSE_ICON_PATH), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.20f, 0.20f, 0.6f)); //标题栏
				ImGui::AlignTextToFramePadding();
				ImGui::TextColored(ImVec4(0.1f, 0.74f, 0.61f, 1.0f), "P A U S E");
				ImGui::Separator();
				ImGui::PopStyleColor();
				ImGui::NewLine();
				ConfigModal::createSoundCombo();

				ImVec2 BtnSize(ImGui::GetColumnWidth(), h / 10.f);
				if (ImGui::Button("Continue", BtnSize)) {
					ImGui::CloseCurrentPopup();
					PauseMenu::visable = false;
					game.state = GAME_STATE::STARTED;
					game.pause = false;

					MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

					UI::UIManager::setCursorVisable(false);

				}

				if (ImGui::Button("Back To Main", BtnSize)) {
					ImGui::CloseCurrentPopup();
					PauseMenu::visable = false;
					backToMain();

				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();
		}
	}



	void ConfigModal::init(GLFWwindow* window) {

	}

	void ConfigModal::draw(unsigned int w, unsigned int h) {
		if (ConfigModal::visable) {
			ImGui::OpenPopup("Config");
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.20f, 0.20f, 0.6f)); //标题栏
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.00f, 0.00f, 0.00f, 0.125f)); //模态框背景
			ImGui::SetNextWindowPos(ImVec2(w / 2 - w / 8, h / 2 - h / 10));
			if (ImGui::BeginPopupModal("Config", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ConfigModal::createSoundCombo();
				ImVec2 closeBtnSize(ImGui::GetColumnWidth(), h / 10.f);
				if (ImGui::Button("Close", closeBtnSize)) {
					MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
					ImGui::CloseCurrentPopup();
					ConfigModal::visable = false;
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor(2);
		}
	}

	void ConfigModal::createSoundCombo() {
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.00f, 0.00f, 0.00f, 0.5f));//弹窗背景
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.00f, 0.00f, 0.0f, 0.25f)); //combo 的hover
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.00f, 0.00f, 0.00f, 0.0f)); //combo 的bg
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.74f, 0.61f, 1.0f)); //combo item 的hover
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.09f, 0.63f, 0.52f, 1.0f)); //combo item 的active(选中)
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.09f, 0.63f, 0.52f, 1.0f)); //combo item 的active(选中)
				// Testing behavior of widgets stacking their own regular popups over the modal.
		ConfigModal::preSoundEffect = ConfigModal::soundEffect;
		ImGui::Combo("Sound Effect", &ConfigModal::soundEffect, "Enable\0Disable\0\0");
		if (ConfigModal::preSoundEffect != ConfigModal::soundEffect) ConfigModal::configSoundEffect();
		ConfigModal::preSoundBg = ConfigModal::soundBg;
		ImGui::Combo("Bg Music", &ConfigModal::soundBg, "Enable\0Disable\0\0");
		if (ConfigModal::preSoundBg != ConfigModal::soundBg) ConfigModal::configSoundBg();
		ImGui::PopStyleColor(6);
	}

	void ConfigModal::configSoundEffect() {
		if (soundEffect == 0) { //开启
			std::cout << "开音效";
		}
		else {//关闭
			std::cout << "关音效";
		}
	}
	void ConfigModal::configSoundBg() {
		if (soundBg == 0) { //开启
			std::cout << "开背景音乐\n";
			MediaPlayer.ResumeBG();
		}
		else {//关闭
			std::cout << "关背景音乐\n";
			MediaPlayer.PauseBackground();
		}
	}

	void HelpModal::init(GLFWwindow* window) {

	}
	void HelpModal::draw(unsigned int w, unsigned int h) {
		if (HelpModal::visable) {
			ImGui::OpenPopup("HELP");
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.20f, 0.20f, 0.6f)); //标题栏
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.00f, 0.00f, 0.00f, 0.125f)); //模态框背景
			ImGui::SetNextWindowPos(ImVec2(w / 2 - w / 8, h / 2 - h / 10));
			if (ImGui::BeginPopupModal("HELP", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text(HETP_TEXT.c_str());
				ImVec2 closeBtnSize(ImGui::GetColumnWidth(), h / 10.f);
				if (ImGui::Button("Close", closeBtnSize)) {
					MediaPlayer.PlayMedia2D(Media::MediaType::CLICK);
					ImGui::CloseCurrentPopup();
					HelpModal::visable = false;
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor(2);
		}
	}

	void CornerTip::init(GLFWwindow* window, Camera* camera) {//左上角
		CornerTip::camera = camera;
		CornerTip::visable = true;
	}
	void CornerTip::draw(unsigned int w, unsigned int h) {
		if (CornerTip::visable) {
			bool* p_open = NULL;
			const float DISTANCE = 10.0f;
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs;
			window_flags |= ImGuiWindowFlags_NoMove;
			ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE);
			ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("Infomation Modal", p_open, window_flags))
			{
				//ImGui::Text("Simple overlay\n" "in the corner of the screen.\n" "(right-click to change position)");
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(CONNER_TIP_POSITION_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(CONNER_TIP_POSITION_TEXT.c_str(), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(CONNER_TIP_ORIENTATION_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(CONNER_TIP_ORIENTATION_TEXT.c_str(), camera->getFront().x, camera->getFront().y, camera->getFront().z);
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(CONNER_TIP_FPS_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(CONNER_TIP_FPS_TEXT.c_str(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}
	}

	void PlayerStatus::init(GLFWwindow* window) {
		PlayerStatus::visable = true;
	}
	void PlayerStatus::draw(unsigned int w, unsigned int h) { //左下角
		if (PlayerStatus::visable) {
			const float X_DISTANCE = 10.0f;
			const float Y_DISTANCE = 60.0f;
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs;
			window_flags |= ImGuiWindowFlags_NoMove;
			ImVec2 window_pos = ImVec2(X_DISTANCE, io.DisplaySize.y - Y_DISTANCE);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.0f, 1.0f));
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("Player Status", NULL, window_flags))
			{
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(PLAYER_STATUS_HP_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(string(PLAYER_STATUS_HP_TEXT + ": %d").c_str(), PlayerStatus::HP);
				ImGui::Separator();
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(PLAYER_STATUS_AMMO_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(string(PLAYER_STATUS_AMMO_TEXT + ": %d").c_str(), PlayerStatus::Ammo);
				ImGui::Separator();
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(PLAYER_STATUS_MISSILE_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(string(PLAYER_STATUS_MISSILE_TEXT + ": %d").c_str(), PlayerStatus::Missile);
			}
			ImGui::End();
		}
	}

	void MissionModal::init(GLFWwindow* window) {
		maxLevel = MAX_GAME_LEVEL;
		visable = true;
		currLevel = 1;
		MissionModal::levelToTaskInfo[1] = GAME_MISSION_TANK_TEXT;
		MissionModal::levelToTaskIcon[1] = GAME_MISSION_TANK_ICON;
		MissionModal::levelToTaskInfo[2] = GAME_MISSION_TOWER_TEXT;
		MissionModal::levelToTaskIcon[2] = GAME_MISSION_TOWER_ICON;
		MissionModal::levelToTaskInfo[3] = GAME_MISSION_PLANE_TEXT;
		MissionModal::levelToTaskIcon[3] = GAME_MISSION_PLANE_ICON;
		currBeatAndTotal = new int*[maxLevel];
		for (int i = 0; i < maxLevel; i++) {
			currBeatAndTotal[i] = new int[2];  //当前击败/目标总数
			currBeatAndTotal[i][0] = currBeatAndTotal[i][1] = 0;
		}

	}
	void MissionModal::draw(unsigned int w, unsigned int h) {
		if (MissionModal::visable) {
			bool* p_open = NULL;
			const float DISTANCE = 10.0f;
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs;
			window_flags |= ImGuiWindowFlags_NoMove;
			ImVec2 window_pos = ImVec2(io.DisplaySize.x - DISTANCE, DISTANCE);
			ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.11f, 0.59f, 0.92f, 0.1f)); //分割线
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
			if (ImGui::Begin("Mission Modal", p_open, window_flags)) {
				ImGui::PushFont(titleFont);
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(GAME_MISSION_ICON), ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::Text(GAME_MISSION_TITLE.c_str());
				ImGui::Separator();
				ImGui::PopFont();

				ImVec4 color(1.f, 1.f, 1.f, 1.f);
				imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				for (int i = 1; i <= maxLevel; i++) {
					if (i > currLevel)
						color.w = 0.6f;
					//	color.x = color.y = color.z = 0.7f;
					ImGui::TextColored(color, "LEVEL %d: Eliminate ", i);
					ImGui::SameLine();
					ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(levelToTaskIcon[i]), ImVec2(imgh, imgh));
					ImGui::SameLine();
					ImGui::TextColored(color, levelToTaskInfo[i].c_str(), currBeatAndTotal[i - 1][0], currBeatAndTotal[i - 1][1]); //// AA on north island
					ImGui::SameLine();
					if (i < currLevel)
						ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(GAME_MISSION_COMPLETE_ICON), ImVec2(imgh, imgh));
					else
						ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(GAME_MISSION_DISCOMPLETE_ICON), ImVec2(imgh, imgh));
				}

			}
			ImGui::End();
			ImGui::PopStyleColor();
		}

	}




	void CenterText::init(GLFWwindow* window) {
		CenterText::visable = false;
	}
	void CenterText::draw(unsigned int w, unsigned int h) {
		if (CenterText::visable) {
			if (CenterText::isBling && blingTimes < CENTER_TEXT_MAX_BLING_TIMES) {
				blingValue += blingDown ? -CENTER_TEXT_BLING_VELOCITY : CENTER_TEXT_BLING_VELOCITY;
				if (blingValue >= 1.f) {
					blingDown = true;  blingValue = 1.f;
				}
				if (blingValue <= 0.f) {
					blingDown = false; blingValue = 0.f; blingTimes++;
				}
			}
			clock_t now = clock();
			if (int(now - showTime - fadeTime) > 0) {
				currAlpha = 1.f - ((now - showTime - fadeTime) * 1.f / (timeToLeave - fadeTime));
			}
			ImVec4 textColor(1.f, blingValue, blingValue, currAlpha);


			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
			window_flags |= ImGuiWindowFlags_NoMove;
			ImVec2 textWinSize = ImVec2(io.DisplaySize.x, CENTER_TEXT_HEIGHT);
			ImGui::SetNextWindowPos(ImVec2(0.0f, CENTER_TEXT_Y_OFFSET));
			ImGui::SetNextWindowSize(textWinSize);
			ImGui::SetNextWindowBgAlpha(0.f); // Transparent background
			if (ImGui::Begin("Text Window", NULL, window_flags))
			{
				auto windowWidth = ImGui::GetWindowSize().x;
				auto textWidth = ImGui::CalcTextSize(CenterText::text.c_str()).x;
				ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
				ImGui::PushFont(zhFont);
				ImGui::TextColored(textColor, StringUtils::stringToUTF8(CenterText::text).c_str());
				ImGui::PopFont();
				ImGui::End();
			}
			if (currAlpha <= 0.f || clock() - showTime >= timeToLeave) {
				CenterText::visable = false;
			}

		}
	}

	void CenterText::show(std::string text, unsigned int timeToLeave, unsigned int fadeTime, bool isBling) {
		CenterText::currAlpha = 1.f;
		CenterText::isBling = isBling;
		CenterText::text = text;
		CenterText::fadeTime = fadeTime * 1000; //储存毫秒
		CenterText::timeToLeave = timeToLeave * 1000; //储存毫秒
		CenterText::showTime = clock();
		CenterText::blingTimes = 0;
		CenterText::blingValue = isBling ? 0.f : 1.f;
		CenterText::blingDown = !isBling;
		CenterText::visable = timeToLeave != 0;
	}

	void LogoText::init(GLFWwindow* window) {
		LogoText::visable = true;
		LogoText::text = WINDOW_TITLE_EN;
	}
	void LogoText::draw(unsigned int w, unsigned int h) {
		if (LogoText::visable) {
			ImVec4 textColor(1.f, 1.f, 1.f, 1.f);
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
			window_flags |= ImGuiWindowFlags_NoMove;
			//ImVec2 textWinSize = ImVec2(io.DisplaySize.x, CENTER_TEXT_HEIGHT);
			ImGui::SetNextWindowPos(ImVec2(200.0f, CENTER_TEXT_Y_OFFSET));
			//	ImGui::SetNextWindowSize(textWinSize);
			ImGui::SetNextWindowBgAlpha(0.f); // Transparent background
			if (ImGui::Begin("Logo Text", NULL, window_flags))
			{
				ImGui::PushFont(logoFont);
				ImGui::TextColored(textColor, LogoText::text.c_str());
				ImGui::PopFont();
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::SameLine();
				ImGui::Image((void*)(intptr_t)TextureManager::getTextureID(GAME_MISSION_COMPLETE_ICON), ImVec2(imgh, imgh));
				ImGui::End();
			}

		}
	}



	void OverModal::init(GLFWwindow* window) {
		isFail = true;
	}

	void OverModal::draw(unsigned int w, unsigned int h) {
		if (OverModal::visable) {
			ImGui::OpenPopup("OVER");
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.00f, 0.00f, 0.00f, 0.125f)); //模态框背景
			ImGui::SetNextWindowPos(ImVec2(w / 2 - w / 6, h / 2 - h / 6));
			if (ImGui::BeginPopupModal("OVER", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
			{
				unsigned int overIconId = TextureManager::getTextureID(isFail ? OVER_ICON_FAIL_PATH : OVER_ICON_COMPLETE_PATH);
				string title = isFail ? OVER_TITLE_FAIL_TEXT : OVER_TITLE_COMPLETE_TEXT;
				ImVec4 titieColor = isFail ? ImVec4(1.f, 0.f, 0.f, 0.9f) : ImVec4(0.f, 1.f, 0.f, 0.9f);
				string content = isFail ? OVER_CONTENT_FAIL_TEXT : OVER_CONTENT_COMPLETE_TEXT;
				float imgh = ImGui::GetTextLineHeightWithSpacing() - ImGui::GetStyle().FramePadding.y;
				ImGui::Image((void*)(intptr_t)overIconId, ImVec2(imgh, imgh));
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.20f, 0.20f, 0.20f, 0.6f)); //标题栏
				ImGui::AlignTextToFramePadding();
				ImGui::TextColored(titieColor, title.c_str());
				ImGui::Separator();
				ImGui::PopStyleColor();
				ImGui::NewLine();
				ImGui::Text(content.c_str());

				ImVec2 BtnSize(w / 3, h / 3.f);
				if (ImGui::Button(OVER_BUTTON1_TEXT.c_str(), BtnSize)) {
					ImGui::CloseCurrentPopup();
					OverModal::visable = false;
					backToMain();
				}
				ImGui::EndPopup();
			}
			ImGui::PopStyleColor();
		}
	}



};