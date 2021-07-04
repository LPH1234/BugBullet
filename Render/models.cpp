
#include"models.h"
#include"Render.h"
#include<ctime>

extern float* createUniformRandomFloatArray(int num, float bottom, float up);
extern float* createNormalRandomFloatArray(int num, float arg1, float arg2);

//点精灵渲染粒子
BaseSpriteParticle::BaseSpriteParticle(glm::vec3 pos, int pointNum, float pointSize, float radis, std::string texturePath, Shader* shader) :PlainModel(pos, glm::vec3(1.f), "", shader) {
	this->hasTexture = texturePath != "";
	this->pointNum = pointNum;
	this->pointSize = pointSize;
	this->radis = radis;
}

bool BaseSpriteParticle::isRemoveable() {
	return this->readyToLeave;
}

FlameParticle::FlameParticle(glm::vec3 pos, int pointNum, float pointSize, float radis, float vy, float maxY, int timeToLeave, std::string texturePath, Shader* shader) :BaseSpriteParticle(pos, pointNum, pointSize, radis, texturePath, shader) {
	this->VY = vy;
	this->maxY = maxY;
	this->timeToLeave = timeToLeave;
	this->createTime = clock();
	//顶点属性
	const int STEP = 3;
	float* vertices = new float[pointNum * STEP]; // x y z
	float* random = createUniformRandomFloatArray(pointNum * STEP, -1.f, 1.f);
	//float* random = createNormalRandomFloatArray(pointNum * STEP, 0.f, 1.f);
	//Logger::debug("pos:", pos);
	//Logger::debug("pos:", Position);
	for (int i = 0; i < pointNum; i++)
	{
		vertices[STEP * i] = random[STEP * i] * radis + pos.x;
		vertices[STEP * i + 1] = pos.y;
		vertices[STEP * i + 2] = random[STEP * i + 2] * radis + pos.z;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointNum * STEP, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STEP * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	if (hasTexture)
		this->textureId = TextureManager::getTextureID(texturePath);
	delete random;
	delete vertices;
}

FlameParticle::~FlameParticle() {
	glDeleteBuffers(1, &this->VBO);
	glDeleteVertexArrays(1, &this->VAO);
}

void FlameParticle::draw() {
	dy += VY;
	if (clock() - createTime > timeToLeave * 1000) { this->readyToLeave = true; return; }
	alpha = 1 - (clock() - createTime) / (timeToLeave * 1000.f);
	shader->setFloat("dy", dy);
	shader->setFloat("pointSize", pointSize);
	shader->setVec3("initPos", Position);
	shader->setFloat("radis", radis);
	shader->setFloat("maxY", maxY);
	shader->setFloat("alpha1", alpha);
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
	float* vertices = new float[pointNum * STEP]; // x y z r
	float* random = createNormalRandomFloatArray(pointNum * STEP, 0.f, 1.f);

	srand(clock());
	for (int i = 0; i < pointNum; i++)
	{
		vertices[STEP * i] = random[STEP * i] * radis + pos.x;
		vertices[STEP * i + 1] = random[STEP * i + 1] * radis + pos.y;
		vertices[STEP * i + 2] = random[STEP * i + 2] * radis + pos.z;
		vertices[STEP * i + 3] = rand() / 65535.f;
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
		this->textureId = TextureManager::getTextureID(texturePath);
	delete random;
	delete vertices;
}

SmokeParticle::~SmokeParticle() {
	glDeleteBuffers(1, &this->VBO);
	glDeleteVertexArrays(1, &this->VAO);
}

void SmokeParticle::draw() {
	dy += VY;
	if (dy > maxY) { this->readyToLeave = true; return; }
	shader->setFloat("dy", dy);
	shader->setFloat("pointSize", pointSize);
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

CloudParticle::CloudParticle(glm::vec3 pos, glm::vec3 scale_value, int cloudDensity, float radis, float vy, float maxY, std::string texturePath, Shader* shader) : BaseSpriteParticle(pos, pointNum, pointSize, radis, texturePath, shader) {
	this->texturePath = texturePath;
	this->cloudDensity = cloudDensity;
	this->radis = radis;
	this->vy = vy;
	this->maxY = maxY;
	this->index = PointerUtils::getPtrIntValue(this);
	this->scale_value = scale_value;

	if (hasTexture)
		this->textureId = TextureManager::getTextureID(texturePath);

	srand(this->index);

	angles = new float[cloudDensity];
	axis = new glm::vec3[cloudDensity];
	trans = new glm::vec3[cloudDensity];
	for (int i = 0; i < cloudDensity; i++)
	{
		angles[i] = rand() * 360.f / RAND_MAX;
		axis[i].x = rand() * 1.f / RAND_MAX;
		axis[i].y = rand() * 1.f / RAND_MAX;
		axis[i].z = rand() * 1.f / RAND_MAX;
	}
	float* random = createNormalRandomFloatArray(3 * cloudDensity, 0.f, radis);
	for (int i = 0; i < cloudDensity; i++)
	{
		trans[i].x = random[i * 3];
		trans[i].y = random[i * 3 + 1];
		trans[i].z = random[i * 3 + 2];
	}
	const int STEP = 5;
	GLfloat transparentVertices[] = {
		// Positions         // Texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	delete random;
}
CloudParticle::~CloudParticle() {
	glDeleteBuffers(1, &this->VBO);
	glDeleteVertexArrays(1, &this->VAO);
	delete angles;
	delete axis;
	delete trans;
}

void CloudParticle::draw() {
	dy += vy;
	if (dy > maxY) { this->readyToLeave = true; return; }
	shader->setFloat("dy", dy);
	shader->setFloat("maxY", maxY);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBindVertexArray(VAO);
	for (int i = 0; i < this->cloudDensity; i++)
	{
		cloudIndex = i;
		this->updateShaderModel();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

glm::mat4 CloudParticle::getModel() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position + trans[cloudIndex]); // 平移到Position
	model = glm::rotate(model, glm::radians(angles[cloudIndex]), glm::normalize(axis[cloudIndex])); //绕y轴旋转,如果rorate_angle是角度的形式
	//model = glm::rotate(model, this->index * 1.f , glm::vec3(0.f,1.f,0.f)); //绕轴y旋转，如果rorate_angle是数字形式
	model = glm::scale(model, scale_value);	//  缩放为scale_value
	return model;
}

//unsigned int CloudParticle::getRandomTextureId(unsigned int index) {
//	if (this->textureIds.size() == 1) this->textureIds[0];
//	return this->textureIds[index % this->textureIds.size()];
//}

// 点精灵渲染粒子end














// 物理渲染粒子

BaseParticle::BaseParticle(glm::vec3 scale, Shader* shader, std::string modelPath) : PlainModel(glm::vec3(0.f, 0.f, 0.f), scale, modelPath, shader) {}
BaseParticle::~BaseParticle() {}
void BaseParticle::update(const PxVec3& position, const PxVec3& velocity) {};
void BaseParticle::setParticleSystem(PxParticleSystem* ps) { this->ps = ps; }


DebrisParticle::DebrisParticle(glm::vec3 scale, vector<string>& modelPathes, glm::vec3 c, Shader* shader) :BaseParticle(scale, shader, "") {
	this->modelPathes = modelPathes;
}

DebrisParticle::~DebrisParticle() {
	glDeleteBuffers(1, &this->VBO);
	glDeleteVertexArrays(1, &this->VAO);
}

void DebrisParticle::update(const PxVec3& position, const  PxVec3& velocity) {
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
	//3.旋转轴和角度
	axisAndAngle.x = data->axisAndAngle->x;
	axisAndAngle.y = data->axisAndAngle->y;
	axisAndAngle.z = data->axisAndAngle->z;
	axisAndAngle.w = data->axisAndAngle->w;
	angle += 1;
	//4.id
	this->id = PointerUtils::getPtrIntValue(ps);
}

void DebrisParticle::draw(unsigned int index, glm::mat4 view, glm::mat4 projection) {
	this->shader->use();
	this->shader->setMat4("projection", projection);
	this->shader->setMat4("view", view);
	this->updateShaderModel();
	index += this->id;
	ModelManager::getModel(this->modelPathes[index%this->modelPathes.size()])->Draw(*shader);
	shader->setFloat("alpha", 1.f);
}

glm::mat4 DebrisParticle::getModel() {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(0.f));
	//model = glm::rotate(model, glm::radians(axisAndAngle.w + angle), glm::normalize(glm::vec3(axisAndAngle.x, axisAndAngle.y, axisAndAngle.z)));
	//model = glm::rotate(model, glm::radians(axisAndAngle.w ), glm::normalize(glm::vec3(axisAndAngle.x, axisAndAngle.y, axisAndAngle.z)));
	model = glm::translate(model, Position);
	model = glm::scale(model, scale_value);//
	return model;
}

// 物理渲染粒子end
