#include"RenderUtils.h"

unsigned int TextureFromFile(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
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
		Logger::error("Texture failed to load at path: " + filename);
		SOIL_free_image_data(data);
	}

	return textureID;
}



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
		std::cout << path << "   load texture start\n";
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << path << "   load texture over\n";
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		Logger::error("Texture failed to load at path: " + string(path));
		SOIL_free_image_data(data);
		*textureID = 0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

}

void loadTexture(char const* path, unsigned char* data, unsigned int* textureID, int width, int height, int nrComponents) {
	glGenTextures(1, textureID);

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

	}
	else
	{
		Logger::error("Texture failed to load at path: " + string(path));
		*textureID = 0;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(data);
}


/*
 * ¼ÓÔØÒ»¸öcubeMap
 */
GLuint loadCubeMapTexture(std::vector<std::string> picFilePathVec,
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
		imageData = SOIL_load_image(picFilePathVec[i].c_str(), &picWidth,
			&picHeight, &channels, loadChannels);
		if (imageData == NULL)
		{
			Logger::error("Error::loadCubeMapTexture could not load texture file:" + string(picFilePathVec[i]));
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