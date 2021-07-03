#ifndef UI_H
#define UI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "../Data/Data.h"
#include "../Data/Consts.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include "RenderUtils.h"
#include<unordered_map>

enum UIID {
	MAIN,
	MENU,
	GAME_PASS,
	GAME_OVER,
	HP_BAR,
	DANGER_MASK,
};



class BaseUI {
protected:
	bool visable = true;
	UIID id;
	glm::vec2 position;
	glm::vec2 size;
public:
	BaseUI(UIID id);
	virtual void draw(unsigned int w, unsigned int h) = 0;
	bool getVisable();
	void setVisable(bool v);
	UIID getUIID();
	void setUIID(UIID id);
	glm::vec2 getPos();
	void setPos(glm::vec2 pos);
	glm::vec2 getSize();
	void setSize(glm::vec2 size);
};

class HPBarUI : public BaseUI
{
public:
	// Constructor (inits shaders/shapes)
	HPBarUI(UIID id, float H, std::string texture);
	// Destructor
	~HPBarUI();
	// Renders a defined quad textured with given sprite
	void draw(unsigned int w, unsigned int h);
	void updateProgress(float progress);
	void setEnableAnimate(bool enbale);

private:
	// Render state
	Shader* shader;
	GLuint VAO;
	Shader* HPBarBorderShader;
	Shader* HPBarFillShader;

	// Initializes and configures the quad's buffer and vertex attributes
	unsigned int textureID;
	bool enableAnimate = true;
	float animateProgress = 100.f;
	const float animateVelocity = 0.5f;
	float progress = 100.f;
	float length;
};

class UIManager {

	static unordered_map<UIID, BaseUI*> idToUI;
	static void addUI(BaseUI*);
public:
	static BaseUI* getUI(UIID id);
	static void setUIVisable(UIID id, bool v);
	static bool getUIVisable(UIID id);
	static void init(const float W, const  float H);
	static void draw(const float W, const float H);
};


class MainMenu {
	static GLFWwindow* window;
public:
	static void init(GLFWwindow* window);
	static void draw(unsigned int w, unsigned int h);
	static bool visable;

};


class CornerTip {

public:
	static void init(GLFWwindow* window);
	static void draw(unsigned int w, unsigned int h);
	static bool visable;

};

class ConfigModal {

public:
	static void init(GLFWwindow* window);
	static void draw(unsigned int w, unsigned int h);
	static bool visable;

	static void configSoundEffect();
	static void configSoundBg();

	static int soundEffect;
	static int soundBg;
};

class HelpModal {

public:
	static void init(GLFWwindow* window);
	static void draw(unsigned int w, unsigned int h);
	static bool visable;
};

#endif