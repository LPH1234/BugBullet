#pragma once
#include <irrKlang.h>
using namespace irrklang;

class Media {
private:
	
	ISoundEngine *SoundEngine;
	ISound *Sound;
public:
	enum MediaType {
		START,TOWERSHOOT, TANKSHOOT, PLANEBULLET, PLANEMISSILE, COLLECTION, SUPPLY, EXPLODE
	};
	Media();
	void PlayBackground(Media::MediaType _type);
	void PlayMedia2D(Media::MediaType _type);
	void PlayMedia3D(vec3df pos, Media::MediaType _type);
	void setlistenerpos(vec3df pos, vec3df lookat);
};

