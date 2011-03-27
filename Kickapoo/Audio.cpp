#include "Common.h"
#include "Audio.h"

void Audio::create()
{
	unsigned int version;
	FMOD::System_Create(&system);
	system->getVersion(&version);
		
	if (version < FMOD_VERSION)
		return;
	
	system->init(32, FMOD_INIT_NORMAL, 0);
}

void Audio::release()
{
	system->close();
    system->release();
}

FMOD::Sound* Audio::loadSound(const char* filePath, bool loop)
{
	FMOD::Sound* sound;
	system->createSound(filePath, FMOD_SOFTWARE, 0, &sound);
	if(loop)
		sound->setMode(FMOD_LOOP_NORMAL);
	return sound;
}

FMOD::Sound* Audio::loadStream(const char* filePath)
{
	FMOD::Sound* sound;
	system->createStream(filePath, FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &sound);
	return sound;
}

void Audio::update()
{
	system->update();
}

void Audio::play(FMOD::Sound* sound, float volume, bool stop)
{
	channel->setVolume(volume);
	//channel->setLoopCount(100000);
	system->playSound(FMOD_CHANNEL_FREE, sound, stop, &channel);
	//sound->setLoopCount(1000);
}

void Audio::stopSoud( FMOD::Sound* sound )
{
	system->playSound(FMOD_CHANNEL_FREE, sound, true, &channel);
}