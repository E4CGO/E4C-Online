//! @file PlayerCharacterPattern.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_PATTERN_H__
#define __INCLUDED_PLAYER_CHARACTER_PATTERN_H__

#include <string>

class PlayerCharacter;

/**************************************************************************//**
	@class	PlayerCharacterPattern
	@brief	キャラクターパターン基底クラス
	@par	[説明]
		パターンを応じてプレイヤーキャラクターを設定する基底クラス
*//***************************************************************************/
class PlayerCharacterPattern
{
public:
	PlayerCharacterPattern() {};
	virtual ~PlayerCharacterPattern() = default;

	virtual void Execute(PlayerCharacter* player) = 0;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternGender
	@brief	性別によって違う処理
	@par	[説明]

*//***************************************************************************/
class PlayerCharacterPatternGender : public PlayerCharacterPattern
{
public:
	PlayerCharacterPatternGender(bool isMale) : PlayerCharacterPattern(), m_isMale(isMale) {}

	void Execute(PlayerCharacter* player);
private:
	bool m_isMale;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternSinglePart
	@brief	単一モデルを実装する処理
	@par	[説明]
		汎用型処理
*//***************************************************************************/
class PlayerCharacterPatternSingleModel : public PlayerCharacterPattern
{
public:
	PlayerCharacterPatternSingleModel(const char* filename) : PlayerCharacterPattern(), m_filename(filename) {}

	void Execute(PlayerCharacter* player) override;
private:
	std::string m_filename;
};
#endif //!__INCLUDED_PLAYER_CHARACTER_PATTERN_H__