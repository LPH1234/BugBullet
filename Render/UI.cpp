#include "UI.h"


HPBarUI::HPBarUI(std::string texture, Shader *shader)
{
	this->shader = shader;
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

void HPBarUI::draw(unsigned int w, unsigned int h, glm::vec2 position, glm::vec2 size)
{
	const float ZV = -2.f;
	// Prepare transformations
	this->shader->use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position.x, position.y, ZV));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	//model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, ZV)); // Move origin of rotation to center of quad
	//model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	//model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, ZV)); // Move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	this->shader->setMat4("model", model);
	// Render textured quad
	this->shader->setFloat("progress", progress);
	this->length = 0.339f * size.x * w;
	this->shader->setFloat("length", length);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void HPBarUI::updateProgress(float progress) {
	this->progress = progress;
}