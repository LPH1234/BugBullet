#include "ResourceManager.h"

//储存程序中反复用到的obj模型路径
std::vector<std::string> ModelManager::modelPathes;
std::unordered_map<std::string, Model*> ModelManager::pathToModel;
std::unordered_set<std::string> ModelManager::modelPathSet;

//储存程序中反复用到的纹理路径
std::vector<std::string> TextureManager::texturePathes;
std::unordered_map<std::string, unsigned int> TextureManager::pathToTextureID;
std::unordered_set<std::string>  TextureManager::textureSet;


void ModelManager::init() {
	for (int i = 1; i <= 18; i++){
		modelPathes.push_back("model/particle/crash/" + to_string(i) + ".obj"); //机械残骸碎片
	}

	for (int i = 0; i < modelPathes.size(); i++){
		if (FileUtils::isFileExist(modelPathes[i])) {
			pathToModel[modelPathes[i]] = new Model(modelPathes[i]);
			modelPathSet.insert(modelPathes[i]); 
		}
		else {
			Logger::error("Model does not exist:" + modelPathes[i]);
		}
	}

}

Model* ModelManager::getModel(std::string modelPath) {
	Model* model = nullptr;
	if (FileUtils::isFileExist(modelPath)) {
		if (modelPathSet.find(modelPath) != modelPathSet.end()) {
			model = pathToModel[modelPath];
		}
		else {
			model = new Model(modelPath);
			pathToModel[modelPath] = model;
		}
	}
	else {
		Logger::error("Model does not exist:" + modelPath);
	}
	return model;
}


void TextureManager::initTextures() {

}

unsigned int TextureManager::getTextureID(std::string texturePath) {

	return 0;
}

void TextureManager::getSkyBoxTextures(std::vector<string>& faces) {
	string dir = "sky2"; string suffix = "png";
	faces.push_back("images/skyboxes/" + dir + "/right." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/left." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/bottom." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/top." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/front." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/back." + suffix);
}
