#ifndef UI_H
#define UI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "../Data/Data.h"
#include "../Data/Consts.h"
#include "objLoader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader.h"
#include "RenderUtils.h"
#include "ResourceManager.h"
#include<unordered_map>

namespace UI {

	void initIcon(GLFWwindow* window);
	void initImgUI(GLFWwindow* window);

	enum UIID {
		MAIN,
		MAIN_ANIMATION,
		MENU,
		GAME_PASS,
		GAME_OVER,
		HP_BAR,
		DANGER_MASK,
	};



	class BaseUI {
	protected:
		bool visable = true;
		bool enableAnimate = true;
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
		void setEnableAnimate(bool enable);

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

	private:
		// Render state
		Shader* shader;
		GLuint VAO;
		Shader* HPBarBorderShader;
		Shader* HPBarFillShader;

		// Initializes and configures the quad's buffer and vertex attributes
		unsigned int textureID;
		const float XOFFSET = 10.f;
		const float YOFFSET = 28.f;
		const float WIDTH = 170.f;
		const float HEIGHT = 18.f;
		float animateProgress = 100.f;
		const float animateVelocity = 0.5f;
		float progress = 100.f;
		float length;
	};


	class AnimationUI : public BaseUI {
	public:
		// Constructor (inits shaders/shapes)
		AnimationUI(UIID id, float W, float H, std::vector<unsigned int>* textureIDs, unsigned int startFrameIndex = 0);
		// Destructor
		~AnimationUI();
		// Renders a defined quad textured with given sprite
		void draw(unsigned int w, unsigned int h);
		void setStartFrame(unsigned int);

	private:
		// Render state
		GLuint VAO;
		Shader* AnimationUIShader;
		std::vector<unsigned int>* textureIDs;
		// Initializes and configures the quad's buffer and vertex attributes
		std::string startFramePath;
		unsigned int startFrameIndex = 0;
		unsigned int currFrameIndex = 0;
	};

	class UIManager {

		static unordered_map<UIID, BaseUI*> idToUI;
		static void addUI(BaseUI*);
	public:
		static BaseUI* getUI(UIID id);
		static void setUIVisable(UIID id, bool v);
		static bool getUIVisable(UIID id);
		static void setEnableAnimate(UIID id, bool enable);
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


	class PauseMenu {
		static GLFWwindow* window;
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

		static void createSoundCombo();
		static void configSoundEffect();
		static void configSoundBg();

		static int soundEffect;
		static int preSoundEffect;
		static int soundBg;
		static int preSoundBg;

		static ImGuiStyle* style;
	};

	class HelpModal {

	public:
		static void init(GLFWwindow* window);
		static void draw(unsigned int w, unsigned int h);
		static bool visable;
	};

	class CornerTip {
		static Camera* camera;
		static int fadeTime;
		static int createTime;
	public:
		static void init(GLFWwindow* window, Camera* camera);
		static void draw(unsigned int w, unsigned int h);
		static bool visable;

	};

	class PlayerStatus {
	public:
		static void init(GLFWwindow* window);
		static void draw(unsigned int w, unsigned int h);
		static bool visable;

		static int HP;
		static int Ammo;
		static int Missile;

	};

	class TextModal {
		static std::string text;
	public:
		static void init(GLFWwindow* window);
		static void draw(unsigned int w, unsigned int h);
		static bool visable;
	};


	class OverModal {
		static std::string text;
	public:
		static void init(GLFWwindow* window);
		static void draw(unsigned int w, unsigned int h);
		static bool visable;
	};

}; // namespace UI


#endif