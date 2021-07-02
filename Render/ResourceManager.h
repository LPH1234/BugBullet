#include "Model.h"
#include "../Utils/Utils.h"

#include<unordered_map>
#include<unordered_set>




class ModelManager {
	//��������з����õ���objģ��·��,��ǰ����������
	static std::vector<std::string> modelPathes;
	static std::unordered_map<std::string, Model*> pathToModel;
	static std::unordered_set<std::string> modelPathSet;

public:
	static void initModels();
	static Model* getModel(std::string modelPath);


};

class TextureManager {
	//��������з����õ�������·��,��ǰ����������
	static std::vector<std::string> texturePathes;
	static std::unordered_map<std::string, unsigned int> pathToTextureID;
	static std::unordered_set<std::string> textureSet;
public:
	static void initTextures();
	static unsigned int getTextureID(std::string texturePath);
};
