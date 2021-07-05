#pragma once
#include "Media.h"
using namespace irrklang;
Media::Media() {
	SoundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS);
	Sound = nullptr;
	BGSOUND = nullptr;
}
Media::MediaType Media::getBGtype() {
	return this->currentBG;
}
void Media::PlayBackground(Media::MediaType _type) {
	if (currentBG!= Media::MediaType::START &&_type == Media::MediaType::START) {
		/*Sound=SoundEngine->play2D("audio/getout.ogg", false, false, true, ESM_AUTO_DETECT,true);
		Sound->setVolume(0.5f);*/
		BGSOUND = SoundEngine->play2D("audio/getout.ogg", true, false, true, ESM_AUTO_DETECT);
		BGSOUND->setVolume(0.1);
		currentBG = Media::MediaType::START;
		std::cout <<"play" <<(int)(currentBG)<<"\n" ;
	}
	else if (currentBG != Media::MediaType::FLYING &&_type == Media::MediaType::FLYING) {
		BGSOUND = SoundEngine->play2D("audio/flying1.mp3", true, false, true, ESM_AUTO_DETECT);
		BGSOUND->setVolume(0.1);
		currentBG = Media::MediaType::FLYING;
		std::cout << "play" << (int)(currentBG) << "\n";
	}
}
void Media::StopBackground() {

		if (BGSOUND != nullptr) {
			BGSOUND->setIsPaused(true);
			currentBG = Media::MediaType::NONE;
			std::cout << "stop" << (int)(currentBG) << "\n";
		}

}
void Media::PauseBackground() {
	if (currentBG != Media::MediaType::NONE) {
		std::cout << (int)(currentBG) << "\n";
		if (BGSOUND != nullptr) {
			BGSOUND->setIsPaused(true);
			std::cout << "pause" << (int)(currentBG) << "\n";
		}
	}
}
void Media::ResumeBG() {
	if (currentBG != Media::MediaType::NONE) {
		if (BGSOUND != nullptr) {
			BGSOUND->setIsPaused(false);
			std::cout << "resume" << (int)(currentBG) << "\n";
		}
	}
}
void Media::PlayMedia2D(Media::MediaType _type){
	if (_type == Media::MediaType::EXPLODE) {
		Sound = SoundEngine->play2D("audio/boom.mp3", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::CLICK) {
		Sound = SoundEngine->play2D("audio/load.wav", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3);
	}
	else if (_type == Media::MediaType::TANKSHOOT) {
		Sound = SoundEngine->play2D("audio/tankfire.mp3", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::TOWERSHOOT) {
		Sound = SoundEngine->play2D("audio/biggun1.wav", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::PLANEBULLET) {
		Sound = SoundEngine->play2D("audio/biggun2.wav", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::PLANEMISSILE) {
		Sound = SoundEngine->play2D("audio/rlaunch.wav", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::SUPPLY) {
		Sound = SoundEngine->play2D("audio/life_pickup.flac", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::COLLECTION) {
		Sound = SoundEngine->play2D("audio/gun_reload1.ogg", false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.3f);
	}
}
void Media::PlayMedia3D(vec3df pos, Media::MediaType _type) {
	if (_type == Media::MediaType::EXPLODE) {
		Sound = SoundEngine->play3D("audio/boom.mp3",pos*2,false,false,true,ESM_AUTO_DETECT,true);
		//Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::CLICK) {
		Sound = SoundEngine->play3D("audio/load.wav", pos / 5, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.5);
	}
	else if (_type == Media::MediaType::AIMED) {
		Sound = SoundEngine->play3D("audio/aimed.mp3", pos / 5, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.5);
	}
	else if (_type == Media::MediaType::TANKSHOOT) {
		Sound = SoundEngine->play3D("audio/tankfire.mp3", pos/5, false, false, true, ESM_AUTO_DETECT,true);
	}
	else if (_type == Media::MediaType::TOWERSHOOT) {
		Sound = SoundEngine->play3D("audio/biggun1.wav", pos/5, false, false, true, ESM_AUTO_DETECT, true);
		//Sound->setVolume(50.f);
	}
	else if (_type == Media::MediaType::PLANEBULLET) {
		Sound = SoundEngine->play3D("audio/biggun2.wav", pos/5, false, false, true, ESM_AUTO_DETECT, true);
		//Sound->setVolume(6.7f);
	}
	else if (_type == Media::MediaType::PLANEMISSILE) {
		Sound = SoundEngine->play3D("audio/rlaunch.wav", pos/25, false, false, true, ESM_AUTO_DETECT, true);
		//Sound->setVolume(0.7f);
	}
	else if (_type == Media::MediaType::SUPPLY) {
		Sound = SoundEngine->play3D("audio/life_pickup.mp3", pos/25, false, false, true, ESM_AUTO_DETECT, true);
		//Sound->setVolume(50.f);
	}
	else if (_type == Media::MediaType::COLLECTION) {
		Sound = SoundEngine->play3D("audio/gun_reload1.ogg", pos/5, false, false, true, ESM_AUTO_DETECT, true);
		//Sound->setVolume(0.7f);
	}
}
void Media::setlistenerpos(vec3df pos, vec3df lookat) {
	SoundEngine->setListenerPosition(pos, lookat);
}
