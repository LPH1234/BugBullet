#ifndef UI_H
#define UI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "RenderUtils.h"


class BaseUI {

public:
	virtual void draw(unsigned int w, unsigned int h, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10)) = 0;
	
};

class HPBarUI : public BaseUI
{
public:
	// Constructor (inits shaders/shapes)
	HPBarUI(std::string texture, Shader *shader);
	// Destructor
	~HPBarUI();
	// Renders a defined quad textured with given sprite
	void draw(unsigned int w, unsigned int h, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10));
	void updateProgress(float progress);
private:
	// Render state
	Shader* shader;
	GLuint VAO;
	// Initializes and configures the quad's buffer and vertex attributes
	unsigned int textureID;
	float progress = 100.f;
	float length;
};

#endif