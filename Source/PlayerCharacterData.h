//! @file PlayerCharacterData.h
//! @note プレイヤー保存されているデーター

#ifndef __INCLUDED_PLAYER_CHARACTER_DATA_H__
#define __INCLUDED_PLAYER_CHARACTER_DATA_H__

#include <string>
#include <vector>
#include <fstream>

#include "TAKOEngine/Tool/Singleton.h"
#include "json.hpp"
#include "PlayerCharacterPattern.h"

class PlayerCharacter;
/**************************************************************************//**
	@class	PlayerCharacterData
	@brief	プレイヤー保存されているデータクラス
	@par	[説明]
		単一クラス
*//***************************************************************************/
class PlayerCharacterData : public Singleton<PlayerCharacterData>
{
	friend class Singleton <PlayerCharacterData>;
private:
	// コンストラクタ
	PlayerCharacterData();
	// デストラクタ
	~PlayerCharacterData();
public:
	// 外見パータン
	enum APPEARANCE_PATTERN : uint8_t
	{
		GENDER = 0,				// 性別
		HAIR,					// 髪
		HAIR_COLOR,				// 髪の色パターン
		EYE_COLOR,				// 目の色パターン
		SKIN_COLOR,				// 肌の色パターン
		TOP,					// 上半身服パターン
		TOP_COLOR,				// 上半身服色パターン
		BOTTOM,					// 下半身服パターン
		BOTTOM_COLOR,			// 下半身服色パターン
		ARM_GEAR,				// 腕装備
		ARM_GEAR_COLOR,			// 腕装備色パターン
		LEFT_HAND_EQUIPMENT,	// 左手装備
		RIGHT_HAND_EQUIPMENT,	// 右手装備

		NUM						// カウント
	};

	enum WEAPON_PATTERN_MAIN : uint8_t
	{
		SWORD_WEAK = 0,
		ROD_WEAK,
		SWORD_NORMAL,
		ROD_NORMAL,
		SWORD_STRONG,
		ROD_STRONG,
	};

	enum WEAPON_PATTERN_OFFHAND : uint8_t
	{
		NO_SHIELD = 0,
		SHIELD_WEAK,
		SHIELD_NORMAL,
		SHIELD_STRONG,
	};

	// キャラクター情報
	struct CharacterInfo
	{
		std::string name;
		uint8_t pattern[APPEARANCE_PATTERN::NUM] = {};

		CharacterInfo& operator=(const CharacterInfo& other)
		{
			if (this != &other)
			{
				this->name = other.name.c_str();
				for (int i = 0; i < APPEARANCE_PATTERN::NUM; i++)
				{
					this->pattern[i] = other.pattern[i];
				}
			}
			return *this;
		}
	};

	// 現在キャラクターを取得
	const CharacterInfo& GetCurrentCharacter() const
	{
		return m_CharaterInfosData[m_CurrentSaveState];
	}
	// 現在キャラクターを設定
	void SetCurrentCharacter(int index)
	{
		m_CurrentSaveState = index;
	}
	// 外見情報をキャラクターに登録
	void LoadAppearance(PlayerCharacter* chara, uint8_t appearance_idx, uint8_t pattern_idx);

	const CharacterInfo& GetCharacterInfo(int idx) const { return m_CharaterInfosData[idx]; }
	void SetCharacterInfo(int idx, const CharacterInfo info) {
		if (idx >= m_CharaterInfosData.size())
		{
			m_CharaterInfosData.push_back(info);
		}
		else
		{
			m_CharaterInfosData[idx] = info;
		}
	}
	const std::vector<CharacterInfo>& GetCharacterInfosData() const { return m_CharaterInfosData; }
	void SetCharacterInfosData(std::vector<CharacterInfo> savedData) { m_CharaterInfosData = savedData; }

	bool LoadData()
	{
		m_CharaterInfosData.clear();
		nlohmann::json j;
		std::ifstream file("CharacterInfos.json");
		if (file.is_open())
		{
			file >> j;

			for (const auto& jCharacter : j["Characters"]) {
				m_CharaterInfosData.push_back({
						jCharacter["NAME"],						// 名前
						{										// 外見
							jCharacter["GENDER"],				// 性別
							jCharacter["HAIR"],					// 髪
							jCharacter["HAIR_COLOR"],			// 髪の色パターン
							jCharacter["EYE_COLOR"],			// 目の色パターン
							jCharacter["SKIN_COLOR"],			// 肌の色パターン
							jCharacter["TOP"],					// 上半身服パターン
							jCharacter["TOP_COLOR"],			// 上半身服色パターン
							jCharacter["BOTTOM"],				// 下半身服パターン
							jCharacter["BOTTOM_COLOR"],			// 下半身服色パターン
							jCharacter["ARM_GEAR"],				// 腕装備
							jCharacter["ARM_GEAR_COLOR"],		// 腕装備色パターン
							jCharacter["LEFT_HAND_EQUIPMENT"],	// 左手装備
							jCharacter["RIGHT_HAND_EQUIPMENT"],	// 右手装備
						}
					});
			}
			file.close();
			return true;
		}
		return false;
	}

	bool SaveData()
	{
		nlohmann::json j;
		j["Characters"] = nlohmann::json::array();
		for (const CharacterInfo& characterInfo : m_CharaterInfosData)
		{
			j["Characters"].push_back({
				{ "NAME", characterInfo.name },
				{ "GENDER", characterInfo.pattern[APPEARANCE_PATTERN::GENDER] },				// 性別
				{ "HAIR", characterInfo.pattern[APPEARANCE_PATTERN::HAIR] },					// 髪
				{ "HAIR_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::HAIR_COLOR] },			// 髪の色パターン
				{ "EYE_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::EYE_COLOR] },			// 目の色パターン
				{ "SKIN_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::SKIN_COLOR] },			// 肌の色パターン
				{ "TOP", characterInfo.pattern[APPEARANCE_PATTERN::TOP] },					// 上半身服パターン
				{ "TOP_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::TOP_COLOR] },			// 上半身服色パターン
				{ "BOTTOM", characterInfo.pattern[APPEARANCE_PATTERN::BOTTOM] },				// 下半身服パターン
				{ "BOTTOM_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::BOTTOM_COLOR] },			// 下半身服色パターン
				{ "ARM_GEAR", characterInfo.pattern[APPEARANCE_PATTERN::ARM_GEAR] },				// 腕装備
				{ "ARM_GEAR_COLOR", characterInfo.pattern[APPEARANCE_PATTERN::ARM_GEAR_COLOR] },		// 腕装備色パターン
				{ "LEFT_HAND_EQUIPMENT", characterInfo.pattern[APPEARANCE_PATTERN::LEFT_HAND_EQUIPMENT] },	// 左手装備
				{ "RIGHT_HAND_EQUIPMENT", characterInfo.pattern[APPEARANCE_PATTERN::RIGHT_HAND_EQUIPMENT] },	// 右手装備
				});
		}

		std::ofstream output_file("CharacterInfos.json");
		if (output_file.is_open()) {
			output_file << j.dump(4);  // インデント付きで書き込む
			output_file.close();
			return true;
		}
		return false;
	}

	const size_t GetAppearancePatternSize(uint8_t idx) { return m_pAppearancePatterns[idx].size(); }

	// 外見パターンをクリア
	void ClearAppearancePatterns();

	void SetMalePatterns();
	void SetFemalePatterns();

private:
	int m_CurrentSaveState = 0;

	std::unordered_map<uint8_t, std::vector<PlayerCharacterPattern*>> m_pAppearancePatterns;
	std::vector<CharacterInfo> m_CharaterInfosData;
};

#define PLAYER_CHARACTER_DATA PlayerCharacterData::Instance()

#endif __INCLUDED_PLAYER_CHARACTER_DATA_H__ // __INCLUDED_PLAYER_CHARACTER_DATA_H__