#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include "p2SString.h"

#define DEFAULT_MUSIC_FADE_TIME 0.5f

struct sfx
{
	p2SString file;
	int id;
};

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();

	bool Start();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

public:
	void SetMusicVolume(int volume) {
		musicVolume = (unsigned __int8)volume;
		Mix_VolumeMusic(masterVolume * (musicVolume * MIX_MAX_VOLUME / 100) / 100);
	}

public:
	sfx goal;
	sfx win;
	sfx lose;

private:
	p2SString musicFolder = "audio/music/";
	p2SString sfxFolder = "audio/sfx/";

	Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;

	unsigned char masterVolume = 100;
	unsigned char musicVolume = 100;
};

#endif // __ModuleAudio_H__