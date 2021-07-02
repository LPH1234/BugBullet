#include "Model.h"
#include "../Utils/Utils.h"

#include<unordered_map>
#include<unordered_set>




class ModelManager {
	//储存程序中反复用到的obj模型路径,提前创建并缓存
	static std::vector<std::string> modelPathes;
	static std::unordered_map<std::string, Model*> pathToModel;
	static std::unordered_set<std::string> modelPathSet;

public:
	static void initModels();
	static Model* getModel(std::string modelPath);


};

class TextureManager {
	//储存程序中反复用到的纹理路径,提前创建并缓存
	static std::vector<std::string> texturePathes;
	static std::unordered_map<std::string, unsigned int> pathToTextureID;
	static std::unordered_set<std::string> textureSet;
public:
	static void initTextures();
	static unsigned int getTextureID(std::string texturePath);
};
