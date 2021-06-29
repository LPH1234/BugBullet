
#include"models.h"
#include"Render.h"
#include<ctime>

extern float* createUniformRandomFloatArray(int num, float bottom, float up);
extern float* createNormalRandomFloatArray(int num, float arg1, float arg2, float*);

BaseSpriteParticle::BaseSpriteParticle(glm::vec3 pos, int pointNum, float pointSize, float radis, std::string texturePath, Shader* shader) :PlainModel(pos, glm::vec3(1.f), "", shader) {
	this->hasTexture = texturePath != "";
	this->pointNum = pointNum;
	this->pointSize = pointSize;
	this->radis = radis;
}

bool BaseSpriteParticle::isRemoveable() {
	return this->readyToLeave;
}

FlameParticle::FlameParticle(glm::vec3 pos, int pointNum, float pointSize, float radis, float vy, float maxY, std::string texturePath, Shader* shader):BaseSpriteParticle(pos, pointNum,pointSize,radis,texturePath,shader){
	this->VY = vy;
	this->maxY = maxY;
	//顶点属性
	const int STEP = 3;
	float* vertices = new float[pointNum * STEP]; // x y z
	srand(clock());
	//float* random = new float[pointNum * STEP];
	//createNormalRandomFloatArray(pointNum * STEP, 0.f, 1.f, random);
	float* random = createUniformRandomFloatArray(pointNum * STEP, 0.f, 1.f);
	//float* random = createNormalRandomFloatArray(pointNum * STEP, 0.f, 1.f);
	for (int i = 0; i < pointNum; i++)
	{
		/*int rand1 = rand();
		int rand2 = rand();
		vertices[STEP * i] = (rand1 & 1 ? -1 : 1)* rand1 / 65535.f * radis + pos.x;
		vertices[STEP * i + 1] = pos.y;
		vertices[STEP * i + 2] = (rand2 & 1 ? -1 : 1)* rand2 / 65535.f * radis + pos.z;*/


		float r0 = random[STEP * i];
		float r1 = random[STEP * i + 1];
		float r2 = random[STEP * i + 2];

		vertices[STEP * i] = r1 * 2.5f;
		vertices[STEP * i + 1] = pos.y;
		vertices[STEP * i + 2] = r2 * 2.5f;

		/*std::cout << "r0 " << r0 << endl;
		std::cout << "r1 " << r1 << endl;
		std::cout << "r2 " << r2 << endl;*/



	/*	std::cout << "i:" << random[STEP * i] << endl;
		std::cout << "i+1:" << random[STEP * i + 1] << endl;
		std::cout << "i+2:" << random[STEP * i + 2] << endl;*/

	}
	

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointNum * STEP, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STEP * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	if (hasTexture)
		loadTextureRGBA(texturePath.c_str(), &textureId);
	std::cout << "over\n";
	delete random;
	std::cout << "over down\n";
	std::cout << "random size:" << pointNum * STEP << "\n";
}


void FlameParticle::draw() {
	dy += VY;
	//if ( timeToLeave ) { this->readyToLeave = true; return; }
	shader->setFloat("dy", dy);
	shader->setFloat("pointSize", pointSize);
	shader->setVec3("initPos", Position);
	shader->setFloat("radis", radis);
	shader->setFloat("maxY", maxY);
	this->updateShaderModel();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (hasTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointNum);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 0);
}

SmokeParticle::SmokeParticle(glm::vec3 pos, int pointNum, float pointSize, float radis, float vy, float maxY, std::string texturePath, Shader* shader) :BaseSpriteParticle(pos, pointNum, pointSize, radis, texturePath, shader) {
	this->VY = vy;
	this->maxY = maxY;
	//顶点属性
	const int STEP = 4;
	float* vertices = new float[pointNum * STEP]; // x y z r g b a
	srand(clock());
	for (int i = 0; i < pointNum; i++)
	{
		int rand1 = rand();
		int rand2 = rand();
		int rand3 = rand(); 
		int rand4 = rand(); 
		vertices[STEP * i] = (rand1 & 1 ? -1 : 1)* rand1 * radis / 65535.f + pos.x;
		vertices[STEP * i + 1] = (rand2 & 1 ? -1 : 1)* rand2 * radis / 65535.f + pos.y;
		vertices[STEP * i + 2] = (rand3 & 1 ? -1 : 1)* rand3 * radis / 65535.f + pos.z;
		vertices[STEP * i + 3] = rand4 / 65535.f;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointNum * STEP, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STEP * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, STEP * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	if (hasTexture)
		loadTextureRGBA(texturePath.c_str(), &textureId);
}
void SmokeParticle::draw() {
	dy += VY;
	if (dy > maxY) { this->readyToLeave = true; return; }
	shader->setFloat("dy", dy);
	shader->setFloat("pointSize", pointSize);
	shader->setVec3("initPos", Position);
	shader->setFloat("maxY", maxY);
	this->updateShaderModel();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (hasTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, pointNum);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, 0);
}






BaseParticle::BaseParticle(glm::vec3 scale, Shader* shader) :PlainModel(glm::vec3(0.f, 0.f, 0.f), scale, "", shader) {}
BaseParticle::~BaseParticle() {}
void BaseParticle::update(const PxVec3& position, const PxVec3& velocity) {};
void BaseParticle::setParticleSystem(PxParticleSystem* ps) { this->ps = ps; }


PointParticle::PointParticle(glm::vec3 scale, glm::vec3 c, Shader* shader) :BaseParticle(scale, shader) {
	this->objectColor = c;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	float vertices[] = { 0.f,0.f,0.f };
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

PointParticle::~PointParticle() {}

void PointParticle::update(const PxVec3& position, const  PxVec3& velocity) {
	//1.设置位置
	this->setPosition(glm::vec3(position.x, position.y, position.z));
	//2.设置透明度
	ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);
	float alpha = 1.f;
	clock_t now = clock();
	if (data->deleteDelaySec != -1) { // 根据时间改变alpha值
		if (now - data->createTime > data->fadeDelaySec * 1000)
			alpha = 1.f - (((now - data->createTime - data->fadeDelaySec * 1000) * 1.0f) / ((data->deleteDelaySec - data->fadeDelaySec) * 1000));
		else
			alpha = 1.f;
		alpha = alpha < 1e-6 ? 0 : alpha;
	}
	//std::cout << "alpha:" << alpha << "\n";
	shader->setFloat("alpha", alpha);
}

void PointParticle::draw(unsigned int index, glm::mat4 view, glm::mat4 projection) {
	shader->setVec3("objectColor", this->objectColor);
	this->shader->use();
	this->shader->setMat4("projection", projection);
	this->shader->setMat4("view", view);
	this->updateShaderModel();

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);

	//recover
	shader->setVec3("objectColor", this->defaultColor);
	shader->setFloat("alpha", 1.f);
}

//
//SmokeParticle::SmokeParticle(glm::vec3 scale, glm::vec3 c, Shader* shader, std::vector<std::string>& textures) :BaseParticle(scale, shader) {
//	this->textures = textures;
//	texturePtr = new unsigned int[textures.size()];
//	this->initTextures();
//	this->init();
//}
//SmokeParticle::~SmokeParticle() {
//	delete texturePtr;
//}
//// Update all particles
//void SmokeParticle::update(const PxVec3& position, const PxVec3& velocity) {
//	life = 1.f;
//	PxVec3 p = position;
//	PxVec3 v = velocity;
//	pxVec3ToGlmVec3(p, Position);
//	pxVec3ToGlmVec3(v, Velocity);
//	ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);
//	float alpha = 1.f;
//	clock_t now = clock();
//	if (data->deleteDelaySec != -1) { // 根据时间改变alpha值
//		if (now - data->createTime > data->fadeDelaySec * 1000)
//			alpha = 1.f - (((now - data->createTime - data->fadeDelaySec * 1000) * 1.0f) / ((data->deleteDelaySec - data->fadeDelaySec) * 1000));
//		else
//			alpha = 1.f;
//		alpha = alpha < 1e-6 ? 0 : alpha;
//	}
//	//std::cout << "alpha:" << alpha << "\n";
//	shader->setFloat("alpha", alpha);
//}
//void SmokeParticle::draw(unsigned int index, glm::mat4 view, glm::mat4 projection) {
//	this->currIndex = index;
//	ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);
//	axisAndAngle = data->axisAndAngle[currIndex];
//	angle = axisAndAngle.w;
//	this->shader->use();
//	this->shader->setMat4("projection", projection);
//	this->shader->setMat4("view", view);
//	// Sort windows
//	/*std::map<GLfloat, glm::vec3> sorted;
//	for (GLuint i = 0; i < windows.size(); i++)
//	{
//		GLfloat distance = glm::length(glm::vec3(0.f, 0.f, 0.f) - windows[i]);
//		sorted[distance] = windows[i];
//	}*/
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//
//	glBindTexture(GL_TEXTURE_2D, this->getRandomTextureId(index));
//	glBindVertexArray(VAO);
//	/*for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
//	{
//		glm::mat4 m = glm::translate(this->getModel(), it->second);
//	}*/
//	const int DRAW_TIMES = 5;
//	for (int i = 0; i < DRAW_TIMES; i++)
//	{
//		angle += 360 / DRAW_TIMES * i;
//		this->updateShaderModel();
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//	}
//	glBindVertexArray(0);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	shader->setFloat("alpha", 1.0f);
//}
//
//glm::mat4 SmokeParticle::getModel() {
//	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, Position); // 平移到Position
//	model = glm::rotate(model, glm::radians(angle), glm::normalize(glm::vec3(axisAndAngle.x, axisAndAngle.y, axisAndAngle.z))); //绕y轴旋转,如果rorate_angle是角度的形式
//	//model = glm::rotate(model, this->currIndex * 1.f , glm::vec3(0.f,1.f,0.f)); //绕轴y旋转，如果rorate_angle是数字形式
//	model = glm::scale(model, scale_value);	//  缩放为scale_value
//	return model;
//}
//
//// Initializes buffer and vertex attributes
//void SmokeParticle::init() {
//	// Set up mesh and attribute properties
//	GLfloat transparentVertices[] = {
//		// Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
//		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
//		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
//		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
//
//		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
//		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
//		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
//	};
//
//	glGenVertexArrays(1, &this->VAO);
//	glGenBuffers(1, &this->VBO);
//	glBindVertexArray(this->VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//	glBindVertexArray(0);
//}
//
//void SmokeParticle::initTextures() {
//	for (int i = 0; i < this->textures.size(); i++)
//	{
//		loadTextureRGBA(this->textures[i].c_str(), &texturePtr[i]);
//		if (texturePtr[i] != 0) {
//			this->textureIds.push_back(texturePtr[i]);
//		}
//	}
//}
//unsigned int SmokeParticle::getRandomTextureId(unsigned int index) {
//	if (this->textureIds.size() == 1) this->textureIds[0];
//	return this->textureIds[index % this->textureIds.size()];
//}
//

















void loadTextureRGBA(char const* path, unsigned int* textureID)
{
	glGenTextures(1, textureID);
	int width, height;
	unsigned char* data = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
	if (data)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SOIL_free_image_data(data);
	}
	else
	{
		Logger::error("Texture failed to load at path : " + string(path));
		SOIL_free_image_data(data);
		*textureID = 0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}





void loadTexture(char const* path, unsigned int* textureID)
{

	glGenTextures(1, textureID);

	int width, height, nrComponents;
	unsigned char* data = SOIL_load_image(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, *textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		SOIL_free_image_data(data);
		*textureID = 0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

}


/*
 * 加载一个cubeMap
 */
GLuint loadCubeMapTexture(std::vector<const char*> picFilePathVec,
	GLint internalFormat,
	GLenum picFormat,
	GLenum picDataType,
	int loadChannels)
{
	GLuint textId;
	glGenTextures(1, &textId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textId);
	GLubyte *imageData = NULL;
	int picWidth, picHeight;
	for (std::vector<const char*>::size_type i = 0; i < picFilePathVec.size(); ++i)
	{
		int channels = 0;
		imageData = SOIL_load_image(picFilePathVec[i], &picWidth,
			&picHeight, &channels, loadChannels);
		if (imageData == NULL)
		{
			std::cerr << "Error::loadCubeMapTexture could not load texture file:"
				<< picFilePathVec[i] << std::endl;
			return 0;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, internalFormat, picWidth, picHeight, 0, picFormat, picDataType, imageData);
		SOIL_free_image_data(imageData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textId;
}