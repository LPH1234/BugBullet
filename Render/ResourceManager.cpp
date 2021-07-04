#include "ResourceManager.h"

//��������з����õ���objģ��·��
std::vector<std::string> ModelManager::modelPathes;
std::unordered_map<std::string, Model*> ModelManager::pathToModel;
std::unordered_set<std::string> ModelManager::modelPathSet;

//��������з����õ�������·��
std::vector<std::string> TextureManager::texturePathes;
std::unordered_map<std::string, unsigned int> TextureManager::pathToTextureID;
std::unordered_set<std::string>  TextureManager::textureSet;
std::vector<unsigned int>  TextureManager::animateTextureIDs;
std::list<TextureDataInfo*>  TextureManager::animateTextureDatas;
unsigned int TextureManager::frameNum = 0;
float TextureManager::loadTexProgress = 0.f;

//��������̻߳�����
std::mutex tex_loader_mutex;


void ModelManager::init() {
	for (int i = 1; i <= 18; i++) {
		modelPathes.push_back("model/particle/crash/" + to_string(i) + ".obj"); //��е�к���Ƭ
	}

	for (int i = 0; i < modelPathes.size(); i++) {
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
			modelPathSet.insert(modelPath);
		}
	}
	else {
		Logger::error("Model does not exist:" + modelPath);
	}
	return model;
}


void TextureManager::init() {
	//��������
	for (int i = 0; i <= 4; i++)
		texturePathes.push_back("images/textures/flame/flame" + to_string(i) + ".png");

	//��������
	texturePathes.push_back(GRAY_CLOUD_TEXTURE_PATH);
	texturePathes.push_back(WHITE_CLOUD_TEXTURE_PATH);
	texturePathes.push_back(GRAY_SMOKE_TEXTURE_PATH);

	//�û�״̬��ͼ������
	texturePathes.push_back(PLAYER_STATUS_HP_ICON);
	texturePathes.push_back(PLAYER_STATUS_AMMO_ICON);
	texturePathes.push_back(PLAYER_STATUS_MISSILE_ICON);
	//���Ͻ�״̬��
	texturePathes.push_back(CONNER_TIP_ORIENTATION_ICON);
	texturePathes.push_back(CONNER_TIP_POSITION_ICON);
	//Σ�ձ�Ե����
	texturePathes.push_back(BORDER_MASK_UI_TEX_PATH);

	texturePathes.push_back(CUBE_TEXTURE_PATH);
	texturePathes.push_back(HPBAR_TEXTURE_PATH);
	texturePathes.push_back(PAUSE_ICON_PATH);
	texturePathes.push_back(OVER_ICON_PATH);

	for (int i = 0; i < texturePathes.size(); i++) {
		if (FileUtils::isFileExist(texturePathes[i])) {
			unsigned int *id = new unsigned int;
			loadTextureRGBA(texturePathes[i].c_str(), id);
			pathToTextureID[texturePathes[i]] = *id;
			textureSet.insert(texturePathes[i]);
		}
		else {
			Logger::error("Texture does not exist:" + texturePathes[i]);
		}
	}

	char* currDir = new char[1024];
	getcwd(currDir, 1024);
	string imagesDir = string(currDir) + "/" + ANIMATE_FRAMS_PATH;
	StringUtils::replaceChar(imagesDir, '/', '\\'); imagesDir.pop_back();
	TextureManager::frameNum = FileUtils::getFilesCount(imagesDir);
	delete currDir;
}

unsigned int TextureManager::getTextureID(std::string texturePath) {
	unsigned int* id = new unsigned int;
	if (FileUtils::isFileExist(texturePath)) {
		if (textureSet.find(texturePath) != textureSet.end()) {
			*id = pathToTextureID[texturePath];
		}
		else {
			loadTextureRGBA(texturePath.c_str(), id);
			pathToTextureID[texturePath] = *id;
			textureSet.insert(texturePath);
		}
	}
	else {
		Logger::error("Texture does not exist:" + texturePath);
		return 0;
	}
	return *id;
}



DWORD WINAPI runLoadTextures(LPVOID lpParamter) {
	LoadTexTaskInfo* task = (LoadTexTaskInfo*)lpParamter;
	//std::cout << "�߳�:" << task->id << "   start index:" << task->startIndex << "\tend index:" << task->endIndex << "\n";
	for (int i = task->startIndex; i <= task->endIndex; i++) {
		string path = ANIMATE_FRAMS_PATH + to_string(i) + ANIMATE_FRAMS_SUFFIX;
		int width, height, nrComponents;
		unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &nrComponents, 0);
		TextureDataInfo* info = new TextureDataInfo(path.c_str(), data, width, height, nrComponents);
		tex_loader_mutex.lock();
		TextureManager::animateTextureDatas.push_back(info);
		tex_loader_mutex.unlock();

	}
	delete task;
	return 0;
}

void TextureManager::initAnimateTextures() {

	Logger::notice("����֡����" + to_string(frameNum));
	if (TextureManager::frameNum == 0) {
		Logger::warn("No Animation Images!");
		return;
	}
	const int THREAD_NUMBER = 1;
	for (int i = 0; i < THREAD_NUMBER; i++) { //�����̼߳���ͼƬ����
		LoadTexTaskInfo* task = new LoadTexTaskInfo(0, 1 + i * TextureManager::frameNum / THREAD_NUMBER, (i + 1) * TextureManager::frameNum / THREAD_NUMBER, &animateTextureIDs);
		HANDLE hThread1 = CreateThread(NULL, 0, runLoadTextures, (void*)task, 0, NULL);
		CloseHandle(hThread1);
	}
}

float TextureManager::getAnimationLoadProgress() {
	if (TextureManager::animateTextureDatas.size() > 0) {
		tex_loader_mutex.lock();
		TextureDataInfo* info = TextureManager::animateTextureDatas.front();
		TextureManager::animateTextureDatas.pop_front();
		tex_loader_mutex.unlock();
		{
			unsigned int* id = new unsigned int;
			loadTexture(info->path, info->data, id, info->width, info->height, info->nrComponents);
			if (0 != *id) {
				TextureManager::animateTextureIDs.push_back(*id);
				TextureManager::loadTexProgress += 1;
			}
		}
		delete info;
	}
	return (1.f * loadTexProgress) / TextureManager::frameNum;
}



void TextureManager::getSkyBoxTextures(std::vector<string>& faces) {
	string dir = "sky"; string suffix = "png";
	faces.push_back("images/skyboxes/" + dir + "/right." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/left." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/bottom." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/top." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/front." + suffix);
	faces.push_back("images/skyboxes/" + dir + "/back." + suffix);
}
