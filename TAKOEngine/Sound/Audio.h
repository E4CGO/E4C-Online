#pragma once

#include <string>
#include <SDL_mixer.h>
#include <unordered_map>

class Audio
{
private:
	Audio() = default;
	~Audio();
public:
	// インスタンス
	static Audio& Instance()
	{
		static Audio instance;
		return instance;
	}
	void Init();
	void SoundVolume(int volume);
	void MusicVolume(int volume);

	Mix_Music* AddMusic(const char* filename);
	Mix_Chunk* AddSound(const char* filename);

public:

private:
	std::unordered_map<std::string, Mix_Music*> musics;
	std::unordered_map<std::string, Mix_Chunk*> sounds;
};
