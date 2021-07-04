#pragma once
#include "Model.h"
#include "../Utils/Utils.h"
#include "../Data/Consts.h"
#include <direct.h>
#include <thread>
#include <mutex>
#include<unordered_map>
#include<unordered_set>


struct LoadTexTaskInfo
{
	int id;
	int startIndex;
	int endIndex;
	std::vector<unsigned int>* animateTextureIDs;
	LoadTexTaskInfo(int id,
		int startIndex,
		int endIndex,
		std::vector<unsigned int>* animateTextureIDs
	) {
		this->id = id;
		this->startIndex = startIndex;
		this->endIndex = endIndex;
		this->animateTextureIDs = animateTextureIDs;
	}
};

struct TextureDataInfo
{
	char const* path;
	unsigned char* data;
	int width;
	int height;
	int nrComponents;
	TextureDataInfo(char const* path, unsigned char* data, int width, int height, int nrComponents) {
		this->path = path; this->data = data; this->width = width; this->height = height; this->nrComponents = nrComponents;
	}
};


class ModelManager {
	//储存程序中反复用到的obj模型路径,提前创建并缓存
	static std::vector<std::string> modelPathes;
	static std::unordered_map<std::string, Model*> pathToModel;
	static std::unordered_set<std::string> modelPathSet;

public:
	static void init();
	static Model* getModel(std::string modelPath);


};

class TextureManager {
	//储存程序中反复用到的纹理路径,提前创建并缓存
	static std::vector<std::string> texturePathes;
	static std::unordered_map<std::string, unsigned int> pathToTextureID;
	static std::unordered_set<std::string> textureSet;
	static unsigned int frameNum;
public:
	static void init();
	static void initAnimateTextures();
	static unsigned int getTextureID(std::string texturePath);
	static void getSkyBoxTextures(std::vector<string>& faces);
	static float getAnimationLoadProgress();

	static float loadTexProgress;
	static std::list<TextureDataInfo*> animateTextureDatas;
	static std::vector<unsigned int> animateTextureIDs; //进入游戏及其主菜单时加载动画纹理，保存纹理id，在合适的时机移除

};

