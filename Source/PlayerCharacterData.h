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


	enum PATTERN_RESOURCE
	{
		NONE,
		MALE,
		FEMALE,
		// 男
		MALE_SWORD_1,
		MALE_SWORD_2,
		MALE_SWORD_3,
		MALE_ROD_1,
		MALE_ROD_2,
		MALE_HAIR_1,
		MALE_HAIR_2,
		MALE_HAIR_3,
		MALE_TOP_1,
		MALE_TOP_2,
		MALE_TOP_3,
		MALE_ARM_1,
		MALE_ARM_2,
		MALE_ARM_3,
		MALE_BOTTOM_1,
		MALE_BOTTOM_2,
		MALE_BOTTOM_3,
		MALE_SHOES_1,
		MALE_SHOES_2,
		MALE_SHOES_3,
		MALE_SHIELD_1,
		MALE_SHIELD_2,
		//女
		FEMALE_SWORD_1,
		FEMALE_SWORD_2,
		FEMALE_SWORD_3,
		FEMALE_ROD_1,
		FEMALE_ROD_2,
		FEMALE_HAIR_1,
		FEMALE_HAIR_2,
		FEMALE_HAIR_3,
		FEMALE_HAIR_4,
		FEMALE_TOP_1,
		FEMALE_TOP_2,
		FEMALE_TOP_3,
		FEMALE_ARM_1,
		FEMALE_BOTTOM_1,
		FEMALE_BOTTOM_2,
		FEMALE_BOTTOM_3,
		FEMALE_SHOES_1,
		FEMALE_SHOES_2,
		FEMALE_SHIELD_1,
		FEMALE_SHIELD_2,

	};

	enum WEAPON_PATTERN_MAIN : uint8_t
	{
		WEAPON_MAIN_SWORD = 0,
		WEAPON_MAIN_SWORD2,
		WEAPON_MAIN_SWORD3,
		WEAPON_MAIN_ROD,
		WEAPON_MAIN_ROD2,
		//WEAPON_MAIN_ROD3,
	};

	enum WEAPON_PATTERN_OFFHAND : uint8_t
	{
		WEAPON_OFFHAND_SHIELD1 = 0,
		WEAPON_OFFHAND_SHIELD2,
		WEAPON_OFFHAND_NONE,
		//WEAPON_OFFHAND_SHIELD3,
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
	std::unordered_map<uint8_t, PlayerCharacterPattern*> m_pAppearancePatternResource;
	std::vector<CharacterInfo> m_CharaterInfosData;
};

#define PLAYER_CHARACTER_DATA PlayerCharacterData::Instance()

#endif __INCLUDED_PLAYER_CHARACTER_DATA_H__ // __INCLUDED_PLAYER_CHARACTER_DATA_H__