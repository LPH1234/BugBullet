
#include"models.h"


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
}


/*
 * ¼ÓÔØÒ»¸öcubeMap
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