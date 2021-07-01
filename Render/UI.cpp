#include "UI.h"


HPBarUI::HPBarUI(std::string texture)
{
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

void HPBarUI::draw(unsigned int w, unsigned int h, glm::vec2 position, glm::vec2 size)
{
	glm::mat4 projection = glm::ortho(0.0f, w*1.f, h*1.f, 0.0f, -1.0f, 1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	const float FILL_WIDTH = 158.f;
	const float FILL_HEIGHT = 14.f;

	/*1£ºhpÌî³ä*/
	this->HPBarFillShader->use();
	glm::vec3 position0(position.x + (size.x - FILL_WIDTH) / 2.f, position.y + (size.y - FILL_HEIGHT) / 2.f, 0.f);
	glm::vec2 size0(FILL_WIDTH, FILL_HEIGHT);
	model = glm::mat4(1.0f);
	model = glm::translate(model, position0);  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::scale(model, glm::vec3(FILL_WIDTH, FILL_HEIGHT, 1.0f)); // border 170 18
	this->length = size0.x;
	this->HPBarFillShader->setMat4("model", model);
	this->HPBarFillShader->setMat4("projection", projection);
	this->HPBarFillShader->setFloat("progress", 50.0f);
	this->HPBarFillShader->setFloat("length", this->length);
	this->HPBarFillShader->setFloat("leftTopX;", position0.x);
	this->HPBarFillShader->setFloat("leftTopY;", position0.y);
	this->HPBarFillShader->setFloat("rightBottomX;", position0.x + size0.x);
	this->HPBarFillShader->setFloat("rightBottomY;", position0.y + size0.y);
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


}

void HPBarUI::updateProgress(float progress) {
	this->progress = progress;
}