#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"
#include "../Utils/Convert.h"
#include "../Utils/Utils.h"
#include "foundation/PxQuat.h"


void loadTexture(char const* path, unsigned int* textureID);
GLuint loadCubeMapTexture(std::vector<const char*> picFilePathVec,
	GLint internalFormat = GL_RGB,
	GLenum picFormat = GL_RGB,
	GLenum picDataType = GL_UNSIGNED_BYTE,
	int loadChannels = SOIL_LOAD_RGB);


enum MOUSE_EVENT_TYPE {
	LCLICK,
	RCLICK,
	MCLICK,
	MOVE
};

class BaseModel {

protected:
	glm::vec3 Position;
	glm::vec3 scale_value;
	glm::vec3 rorate_axis = glm::vec3(0.f, 1.f, 0.f);
	float rorate_angle = 0;

	Shader* shader;
	std::string modelPath;

	Model* model = nullptr;
	int id;
	std::string name;

public:


	BaseModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) {
		this->Position = pos; this->scale_value = scale;  this->shader = shader;   this->modelPath = modelPath;

		if (modelPath.compare("") != 0) {
			this->model = new Model(modelPath);
		}
	}

	virtual ~BaseModel() {
		if (this->model != nullptr)
			delete this->model;
	}

	virtual void draw() = 0;

	virtual glm::mat4 getModel() = 0;

	virtual void ProcessKeyboard(Movement direction, float deltaTime) = 0;

	virtual void ProcessMouse(MOUSE_EVENT_TYPE event) = 0;

	std::string& getModelPath() {
		return modelPath;
	}

	glm::vec3 getPosition() {
		return this->Position;
	}

	void setPosition(glm::vec3 Position) {
		this->Position = Position;
	}

	void setPosition(physx::PxVec3 Position) {
		pxVec3ToGlmVec3(Position, this->Position);
	}

	glm::vec3 getScaleValue() {
		return this->scale_value;
	}

	void setScaleValue(glm::vec3 scale_value) {
		this->scale_value = scale_value;
	}

	void setScaleValue(physx::PxVec3 scale_value) {
		this->scale_value = pxVec3ToGlmVec3(scale_value);
	}

	void setRotate(physx::PxQuat q) {
		physx::PxVec3 tmp;
		q.toRadiansAndUnitAxis(this->rorate_angle, tmp);
		pxVec3ToGlmVec3(tmp, this->rorate_axis);
	}

	void setShader(Shader* shader) {
		this->shader = shader;
	}

	void updateShaderModel() {
		this->shader->setMat4("model", this->getModel());
	}

	void setModel(Model* model) {
		this->model = model;
	}

	int getId() {
		return id;
	}

	void setId(int id) {
		this->id = id;
	}

	std::string getName() {
		return name;
	}

	void setName(std::string name) {
		this->name = name;
	}

};

//一般静止模型，按原样绘制
class PlainModel : public BaseModel
{
public:


	PlainModel(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :BaseModel(pos, scale, modelPath, shader) {

	}

	~PlainModel() {}

	void draw() {
		model->Draw(*shader);
	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position); // 平移到Position
		//model = glm::rotate(model, glm::radians(rorate_angle), rorate_axis); //绕轴rorate_axis旋转,如果rorate_angle是角度的形式
		model = glm::rotate(model, rorate_angle, rorate_axis); //绕轴rorate_axis旋转，如果rorate_angle是数字形式
		model = glm::scale(model, scale_value);	//  缩放为scale_value
		return model;
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {

	}

	void ProcessMouse(MOUSE_EVENT_TYPE event) {

	}



};

class SmokeParticle : public PlainModel
{
	glm::vec3 objectColor;
	glm::vec3 defaultColor = glm::vec3(1.f, 1.f, 1.f);
	unsigned int VBO, VAO;

public:

	SmokeParticle(glm::vec3 pos, glm::vec3 scale, glm::vec3 c, Shader* shader) :PlainModel(pos, scale, "", shader) {
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

	~SmokeParticle() {}

	void draw() {
		/*glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);*/

		shader->setVec3("objectColor", this->objectColor);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		shader->setVec3("objectColor", this->defaultColor);

		/*	glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);*/
	}


};


class Cube : public PlainModel
{

public:
	bool hasTexture;


	Cube(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, std::string texturePath) :PlainModel(pos, scale, modelPath, shader) {
		this->Position = pos;
		this->scale_value = scale;
		this->hasTexture = texturePath != "";
		//顶点属性
		float vertices[] = {
			// positions          // normals           // texture coords
		  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glEnableVertexAttribArray(0);

		glGenTextures(1, &diffuseMap);
		if (hasTexture)
			loadTexture(texturePath.c_str(), &diffuseMap);


	}

	~Cube() {}

	//绘制模型
	void draw() {
		//将实现绘制的代码写在这
		if (hasTexture) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}


	unsigned int VBO, cubeVAO;
	unsigned int diffuseMap;

};



class Sphere : public PlainModel
{

public:
	const GLfloat PI = 3.14159265358979323846f;
	//将球横纵划分成 y_segments*x_segments 的网格
	int y_segments;
	int x_segments;

	Sphere(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader, int y_segments, int x_segments) :PlainModel(pos, scale, modelPath, shader) {
		this->Position = pos;
		this->scale_value = scale;
		this->x_segments = x_segments;
		this->y_segments = y_segments;

		std::vector<float> sphereVertices;
		std::vector<int> sphereIndices;

		/*2-计算球体顶点*/
		//生成球的顶点
		for (int y = 0; y <= y_segments; y++)
		{
			for (int x = 0; x <= x_segments; x++)
			{
				float xSegment = (float)x / (float)x_segments;
				float ySegment = (float)y / (float)y_segments;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				sphereVertices.push_back(xPos);
				sphereVertices.push_back(yPos);
				sphereVertices.push_back(zPos);
			}
		}

		//生成球的Indices
		for (int i = 0; i < y_segments; i++)
		{
			for (int j = 0; j < x_segments; j++)
			{
				sphereIndices.push_back(i * (x_segments + 1) + j);
				sphereIndices.push_back((i + 1) * (x_segments + 1) + j);
				sphereIndices.push_back((i + 1) * (x_segments + 1) + j + 1);
				sphereIndices.push_back(i* (x_segments + 1) + j);
				sphereIndices.push_back((i + 1) * (x_segments + 1) + j + 1);
				sphereIndices.push_back(i * (x_segments + 1) + j + 1);
			}
		}


		/*3-数据处理*/
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		//生成并绑定球体的VAO和VBO
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//将顶点数据绑定至当前默认的缓冲中
		glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

		GLuint element_buffer_object;//EBO
		glGenBuffers(1, &element_buffer_object);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

		//设置顶点属性指针
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//解绑VAO和VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	~Sphere() {}

	//绘制模型
	void draw() {
		//绘制球
		//开启面剔除(只需要展示一个面，否则会有重合)
		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindVertexArray(VAO);
		//使用线框模式绘制
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, x_segments * y_segments * 6, GL_UNSIGNED_INT, 0);
		this->shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
	}

	void setColor(glm::vec3 c) {
		this->shader->setVec3("objectColor", c);
	}

	unsigned int VBO, VAO;
	unsigned int diffuseMap;

};


class SkyBox : public PlainModel
{
public:

	SkyBox(glm::vec3 pos, glm::vec3 scale, std::string modelPath, Shader* shader) :PlainModel(pos, scale, modelPath, shader) {
		this->shader = shader;
		this->Position = pos;
		this->scale_value = scale;

		// 指定包围盒的顶点属性 位置
		GLfloat skyboxVertices[] = {
			// 背面
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, -1.0f, -1.0f,	// B
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, -1.0f,		// C
			1.0f, 1.0f, -1.0f,		// D
			-1.0f, 1.0f, -1.0f,		// A

			// 左侧面
			-1.0f, -1.0f, 1.0f,		// E
			-1.0f, -1.0f, -1.0f,	// B
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, 1.0f,		// F
			-1.0f, -1.0f, 1.0f,		// E

			// 右侧面
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, 1.0f,		// G
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, -1.0f,		// D
			1.0f, -1.0f, -1.0f,		// C

			// 正面
			-1.0f, -1.0f, 1.0f,  // E
			-1.0f, 1.0f, 1.0f,  // F
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			1.0f, -1.0f, 1.0f,  // G
			-1.0f, -1.0f, 1.0f,  // E

			// 顶面
			-1.0f, 1.0f, -1.0f,  // A
			1.0f, 1.0f, -1.0f,  // D
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			-1.0f, 1.0f, 1.0f,  // F
			-1.0f, 1.0f, -1.0f,  // A

			// 底面
			-1.0f, -1.0f, -1.0f,  // B
			-1.0f, -1.0f, 1.0f,   // E
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, -1.0f,   // C
			-1.0f, -1.0f, -1.0f,  // B
		};

		glGenVertexArrays(1, &skyBoxVAOId);
		glGenBuffers(1, &skyBoxVBOId);
		glBindVertexArray(skyBoxVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		// 顶点位置数据
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GL_FLOAT), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		std::vector<const char*> faces;
		faces.push_back("pic/skyboxes/sky/right.jpg");
		faces.push_back("pic/skyboxes/sky/left.jpg");
		faces.push_back("pic/skyboxes/sky/bottom.jpg");
		faces.push_back("pic/skyboxes/sky/top.jpg");
		faces.push_back("pic/skyboxes/sky/front.jpg");
		faces.push_back("pic/skyboxes/sky/back.jpg");

		skyBoxTextId = loadCubeMapTexture(faces);

	}

	~SkyBox() {}

	void draw() {
		/*glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);*/
		glDepthFunc(GL_LESS);
		glBindVertexArray(skyBoxVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextId); // 注意绑定到CUBE_MAP
		glUniform1i(glGetUniformLocation(shader->ID, "skybox"), 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);
		glDepthFunc(GL_LESS);

	}

	glm::mat4 getModel() {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, scale_value);//
		return model;
	}


	GLuint skyBoxVAOId, skyBoxVBOId;
	GLuint cubeTextId;
	GLuint skyBoxTextId;

};










