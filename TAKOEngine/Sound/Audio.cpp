#include "Audio.h"

#include <SDL.h>

Audio::~Audio()
{
}

void Audio::Init()
{
	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
}

void Audio::SoundVolume(int volumne)
{
	Mix_Volume(-1, volumne);
}
void Audio::MusicVolume(int volumne)
{
	Mix_VolumeMusic(volumne);
}

Mix_Music* Audio::AddMusic(const char* filename)
{
	std::string idx = std::string(filename);
	if (musics.find(idx) == musics.end())
	{
		musics[idx] = Mix_LoadMUS(filename);
	}
	return musics[idx];
}
Mix_Chunk* Audio::AddSound(const char* filename)
{
	std::string idx = std::string(filename);
	if (sounds.find(idx) == sounds.end())
	{
		Mix_Chunk* tmpChunk = Mix_LoadWAV(filename);
		if (tmpChunk != nullptr)
		{
			sounds[idx] = tmpChunk;
		}
		else
		{
			return nullptr;
		}
	}
	return sounds[idx];
}