#pragma once
#include <irrKlang.h>
#include<iostream>
using namespace irrklang;

class Media {
private:
	ISoundEngine *SoundEngine;
	ISound *Sound;
	ISound *BGSOUND;

public:
	enum MediaType {
		NONE,START,TOWERSHOOT, TANKSHOOT, PLANEBULLET, PLANEMISSILE, COLLECTION, SUPPLY, EXPLODE,FLYING,CLICK,SELECT
	};
	Media::MediaType currentBG = Media::MediaType::NONE;
	Media();
	Media::MediaType getBGtype();
	void PlayBackground(Media::MediaType _type);
	void StopBackground();
	void PauseBackground();
	void ResumeBG();
	void PlayMedia2D(Media::MediaType _type);
	void PlayMedia3D(vec3df pos, Media::MediaType _type);
	void setlistenerpos(vec3df pos, vec3df lookat);
};

