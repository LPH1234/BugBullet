#pragma once
#include "Media.h"
using namespace irrklang;
Media::Media() {
	SoundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS);
	Sound = nullptr;
}
void Media::PlayBackground(Media::MediaType _type) {
	if (_type == Media::MediaType::START) {
		Sound = SoundEngine->play2D("audio/getout.ogg", false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.5f);
	}
}
void Media::PlayMedia2D(Media::MediaType _type) {
	if (_type == Media::MediaType::EXPLODE) {
		Sound = SoundEngine->play2D("audio/boom.mp3", false, false, true, ESM_AUTO_DETECT);
		Sound->setVolume(0.3f);
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
		Sound = SoundEngine->play3D("audio/boom.mp3", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.3f);
	}
	else if (_type == Media::MediaType::TANKSHOOT) {
		Sound = SoundEngine->play3D("audio/tankfire.mp3", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(70.f);
	}
	else if (_type == Media::MediaType::TOWERSHOOT) {
		Sound = SoundEngine->play3D("audio/biggun1.wav", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(50.f);
	}
	else if (_type == Media::MediaType::PLANEBULLET) {
		Sound = SoundEngine->play3D("audio/biggun2.wav", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(6.7f);
	}
	else if (_type == Media::MediaType::PLANEMISSILE) {
		Sound = SoundEngine->play3D("audio/rlaunch.wav", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.7f);
	}
	else if (_type == Media::MediaType::SUPPLY) {
		Sound = SoundEngine->play3D("audio/life_pickup.mp3", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(50.f);
	}
	else if (_type == Media::MediaType::COLLECTION) {
		Sound = SoundEngine->play3D("audio/gun_reload1.ogg", pos, false, false, true, ESM_AUTO_DETECT, true);
		Sound->setVolume(0.7f);
	}
}
void Media::setlistenerpos(vec3df pos, vec3df lookat) {
	SoundEngine->setListenerPosition(pos, lookat);
}
