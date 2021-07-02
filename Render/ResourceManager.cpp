#include "ResourceManager.h"

//��������з����õ���objģ��·��
std::vector<std::string> ModelManager::modelPathes;
std::unordered_map<std::string, Model*> ModelManager::pathToModel;
std::unordered_set<std::string> ModelManager::modelPathSet;

//��������з����õ�������·��
std::vector<std::string> TextureManager::texturePathes;
std::unordered_map<std::string, unsigned int> TextureManager::pathToTextureID;
std::unordered_set<std::string>  TextureManager::textureSet;


void ModelManager::initModels() {
	for (int i = 1; i <= 18; i++){
		modelPathes.push_back("model/particle/crash/" + to_string(i) + ".obj"); //��е�к���Ƭ
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

