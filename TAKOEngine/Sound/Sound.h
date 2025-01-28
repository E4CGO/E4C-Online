//! @file Sound.h
//! @note 音楽を管理クラス

#ifndef __INCLUDED_SOUND_H__
#define __INCLUDED_SOUND_H__

#include "TAKOEngine/Tool/Singleton.h"

#include <al.h>
#include <alc.h>
#include <minimp3.h>
#include <minimp3_ex.h>

#include <string>
#include <vector>

/**************************************************************************//**
	@class
	@brief   音楽を管理クラス
	@par    音楽をロード、流れ、止めるクラス
*//***************************************************************************/
class Sound : public Singleton<Sound>
{
	friend class Singleton <Sound>;
private:
	Sound() = default;
	~Sound() = default;
public:
	//　音楽情報
	struct SoundData
	{
		int sampleRate;
		int channels;
		std::vector<int16_t> pcmData;
	};

	// 音楽を初期
	void InitAudio();
	//　音楽を設定
	void LoadAudio(const char* filepath);
	// 音楽を流す
	void PlayAudio(int number);
	// 音楽を止める
	void StopAudio(int number);
	// 音楽をクリーンアップ
	void Finalize();

	void SetVolume(float newVolume) {
		if (this->m_volume != newVolume)
		{
			this->m_volume = newVolume;
			alSourcef(m_Sources[0], AL_GAIN, this->m_volume);
		}
	}
	const float GetVolume() { return m_volume; }

private:
	mp3dec_t m_Mp3d;
	mp3dec_file_info_t m_Info;

	ALCdevice* mp_Device;
	ALCcontext* mp_Context;

	std::vector<ALuint> m_Buffers;
	std::vector<ALuint> m_Sources;

	float m_volume = 0.25f;
};

#endif //__INCLUDED_SOUND_H__
