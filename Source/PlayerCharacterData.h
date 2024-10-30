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
	enum APPEARANCE_PATTERN
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

	struct CharacterInfo
	{
		bool visible = false;

		std::string save = "";

		int GetGender() { return this->Character.pattern[APPEARANCE_PATTERN::GENDER]; }

		struct Character
		{
			uint8_t pattern[APPEARANCE_PATTERN::NUM] = {};

			Character& operator=(const Character& other)
			{
				if (this != &other)
				{
					for (int i = 0; i < APPEARANCE_PATTERN::NUM; i++)
					{
						this->pattern[i] = other.pattern[i];
					}
				}
				return *this;
			}
		} Character;

		CharacterInfo& operator=(const CharacterInfo& other)
		{
			if (this != &other)
			{
				this->visible = other.visible;
				this->save = other.save;
				this->Character = other.Character;
			}
			return *this;
		}
	};

	const CharacterInfo& GetCurrentCharacter() const
	{
		return m_CharaterInfosData[m_CurrentSaveState];
	}

	void LoadAppearance(Character* chara, uint8_t appearance_idx, uint8_t pattern_idx);
	nlohmann::json GetCharacterInfos() const { return m_CharacterInfos; }
	void SetCharacterInfos(nlohmann::json savedData) { m_CharacterInfos = savedData; }

	std::vector<CharacterInfo> GetCharacterInfosData() const { return m_CharaterInfosData; }
	void SetCharacterInfosData(std::vector<CharacterInfo> savedData) { m_CharaterInfosData = savedData; }

	void ParseData()
	{
		nlohmann::json json_array = nlohmann::json::array();

		json_array = m_CharacterInfos["Characters"];

		m_CharaterInfosData.resize(json_array.size());

		for (size_t i = 0; i < json_array.size(); i++)
		{
			m_CharaterInfosData[i] = {
				json_array[i]["Visible"],
				json_array[i]["Save"],
				{
					json_array[i]["Character"]["GENDER"],
					json_array[i]["Character"]["HAIR"],
					json_array[i]["Character"]["HAIR_COLOR"],
					json_array[i]["Character"]["EYE_COLOR"],
					json_array[i]["Character"]["SKIN_COLOR"],
					json_array[i]["Character"]["TOP"],
					json_array[i]["Character"]["TOP_COLOR"],
					json_array[i]["Character"]["BOTTOM"],
					json_array[i]["Character"]["BOTTOM_COLOR"],
					json_array[i]["Character"]["ARM_GEAR"],
					json_array[i]["Character"]["ARM_GEAR_COLOR"],
					json_array[i]["Character"]["LEFT_HAND_EQUIPMENT"],
					json_array[i]["Character"]["RIGHT_HAND_EQUIPMENT"],
				}
			};
		}
	}

	void ToJson(nlohmann::json& j, CharacterInfo charInfo)
	{
		j = nlohmann::json{
		{"Character", {
			{"ARM_GEAR",				charInfo.Character.pattern[GENDER]},
			{"ARM_GEAR_COLOR",			charInfo.Character.pattern[HAIR]},
			{"BOTTOM",					charInfo.Character.pattern[HAIR_COLOR]},
			{"BOTTOM_COLOR",			charInfo.Character.pattern[EYE_COLOR]},
			{"EYE_COLOR",				charInfo.Character.pattern[SKIN_COLOR]},
			{"GENDER",					charInfo.Character.pattern[TOP]},
			{"HAIR",					charInfo.Character.pattern[TOP_COLOR]},
			{"HAIR_COLOR",				charInfo.Character.pattern[BOTTOM]},
			{"LEFT_HAND_EQUIPMENT",		charInfo.Character.pattern[BOTTOM_COLOR]},
			{"RIGHT_HAND_EQUIPMENT",	charInfo.Character.pattern[ARM_GEAR]},
			{"SKIN_COLOR",				charInfo.Character.pattern[ARM_GEAR_COLOR]},
			{"TOP",						charInfo.Character.pattern[LEFT_HAND_EQUIPMENT]},
			{"TOP_COLOR",				charInfo.Character.pattern[RIGHT_HAND_EQUIPMENT]}
		}},
		{"Save", charInfo.save},
		{"Visible", charInfo.visible}
		};
	}

	void SaveData()
	{
		nlohmann::json newSave;
		newSave[0]["Characters"];

		for (auto& it : m_CharaterInfosData)
		{
			nlohmann::json newChara;

			if (it.save == "")
			{
				CharacterInfo charInfo = {
				false,			// visible
				"",				// save
				{				//Character
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
				}
				};

				ToJson(newChara, charInfo);

				newSave[0]["Characters"].push_back(newChara);
			}
			else
			{
				CharacterInfo charInfo = it;

				ToJson(newChara, charInfo);

				newSave[0]["Characters"].push_back(newChara);
			}
		}

		std::ofstream file_out("CharacterInfos.json");
		if (file_out.is_open()) {
			file_out.clear();
			file_out << newSave[0].dump(4); //スペース
			file_out.close();
		}
	}

	// 外見パターンをクリア
	void ClearAppearancePatterns();

	int m_CurrentSaveState = 0;
private:
	std::unordered_map<uint8_t, std::vector<PlayerCharacterPattern*>> m_pappearancePatterns;

	nlohmann::json m_CharacterInfos;
	std::vector<CharacterInfo> m_CharaterInfosData;
};

#define PLAYER_CHARACTER_DATA PlayerCharacterData::Instance()

#endif __INCLUDED_PLAYER_CHARACTER_DATA_H__ // __INCLUDED_PLAYER_CHARACTER_DATA_H__