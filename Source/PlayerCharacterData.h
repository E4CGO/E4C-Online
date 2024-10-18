//! @file PlayerCharacterData.h
//! @note プレイヤー保存されているデーター

#ifndef __INCLUDED_PLAYER_CHARACTER_DATA_H__
#define __INCLUDED_PLAYER_CHARACTER_DATA_H__

#include "TAKOEngine/Tool/Singleton.h"
#include "json.hpp"

#include <string>
#include <vector>

class PlayerCharacterData : public Singleton<PlayerCharacterData>
{
	friend class Singleton <PlayerCharacterData>;
private:
	PlayerCharacterData() = default;
	~PlayerCharacterData() = default;
public:

	struct CharacterInfo
	{
		bool visible;

		std::string save;

		struct Character
		{
			int genderType;
			int headType;
			int bodyType;
			int weaponType;

			Character& operator=(const Character& other)
			{
				if (this != &other)
				{
					this->genderType = other.genderType;
					this->headType = other.headType;
					this->bodyType = other.bodyType;
					this->weaponType = other.weaponType;
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

	nlohmann::json GetCharacterInfos() const { return m_CharacterInfos; }
	void SetCharacterInfos(nlohmann::json savedData) { m_CharacterInfos = savedData; }

	std::vector<CharacterInfo> GetCharacterInfosData() const { return m_CharaterInfosData; }
	void SetCharacterInfosData(std::vector<CharacterInfo> savedData) { m_CharaterInfosData = savedData; }

	void SetCharacterVisibility(bool visibility, int number) { this->m_CharaterInfosData[number].visible = visibility; }
	void SetCharacterGenderType(int characterNumber, int value) { this->m_CharaterInfosData[characterNumber].Character.genderType = value; }
	void SetCharacterHeadType(int characterNumber, int value) { this->m_CharaterInfosData[characterNumber].Character.headType = value; }
	void SetCharacterBodyType(int characterNumber, int value) { this->m_CharaterInfosData[characterNumber].Character.bodyType = value; }
	void SetCharacterWeaponType(int characterNumber, int value) { this->m_CharaterInfosData[characterNumber].Character.weaponType = value; }

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
					json_array[i]["Character"]["GenderType"],
					json_array[i]["Character"]["HeadType"],
					json_array[i]["Character"]["BodyType"],
					json_array[i]["Character"]["WeaponType"],
				}
			};
		}
	}

private:

	nlohmann::json m_CharacterInfos;
	std::vector<CharacterInfo> m_CharaterInfosData;
};

#define PLAYER_CHARACTER_DATA PlayerCharacterData::Instance()

#endif __INCLUDED_PLAYER_CHARACTER_DATA_H__ // __INCLUDED_PLAYER_CHARACTER_DATA_H__