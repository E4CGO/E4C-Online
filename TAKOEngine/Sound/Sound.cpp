#include "Sound.h"

/**************************************************************************//**
	@brief    初期
	@param[in]    なし
*//***************************************************************************/
void Sound::InitAudio()
{
	mp_Device = alcOpenDevice(nullptr);

	mp_Context = alcCreateContext(mp_Device, nullptr);

	alcMakeContextCurrent(mp_Context);

	m_Buffers.push_back({});
	m_Sources.push_back({});

	alGenBuffers(1, &m_Buffers.back());

	alGenSources(1, &m_Sources.back());

	mp3dec_init(&m_Mp3d);
}

/**************************************************************************//**
	@brief    ローディング
	@param[in]    const char* ファイルの場所
*//***************************************************************************/
void Sound::LoadAudio(const char* filepath)
{
	const char* mp3File = filepath;

	if (mp3dec_load(&m_Mp3d, mp3File, &m_Info, nullptr, nullptr));
	std::vector<int16_t> pcmData(m_Info.buffer, m_Info.buffer + m_Info.samples);

	sounds.push_back(
		SoundData{
			m_Info.hz,
			m_Info.channels,
			pcmData
		}
	);

	ALenum format = (sounds.back().channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	alBufferData(m_Buffers.back(), format, sounds.back().pcmData.data(), pcmData.size() * sizeof(int16_t), sounds.back().sampleRate);
}

/**************************************************************************//**
	@brief    プレイ
	@param[in]    int 音楽の番号
*//***************************************************************************/
void Sound::PlayAudio(int number) {
	alSourcei(m_Sources[number], AL_BUFFER, m_Buffers[number]);

	alSourcePlay(m_Sources[number]);
}

/**************************************************************************//**
	@brief    止める
	@param[in]    int 音楽の番号
*//***************************************************************************/
void Sound::StopAudio(int number)
{
	alSourceStop(m_Sources[number]);
}

/**************************************************************************//**
	@brief    クリーンアップ
	@param[in]    なし
*//***************************************************************************/
void Sound::Finalize()
{
	for (auto it : m_Sources)
	{
		alDeleteSources(1, &it);
	}

	for (auto it : m_Buffers)
	{
		alDeleteSources(1, &it);
	}

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(mp_Context);
	alcCloseDevice(mp_Device);

	free(m_Info.buffer);
}