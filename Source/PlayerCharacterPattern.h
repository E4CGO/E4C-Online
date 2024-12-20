//! @file PlayerCharacterPattern.h
//! @note

#ifndef __INCLUDED_PLAYER_CHARACTER_PATTERN_H__
#define __INCLUDED_PLAYER_CHARACTER_PATTERN_H__

#include <string>

class PlayerCharacter;

/**************************************************************************//**
	@class	PlayerCharacterPattern
	@brief	プレイヤーキャラクターパターン基底クラス
	@par	[説明]
		プレイヤーの外見パターンでキャラクター要素を登録する基底クラス
*//***************************************************************************/
class PlayerCharacterPattern
{
public:
	PlayerCharacterPattern() {};
	virtual ~PlayerCharacterPattern() = default;

	virtual void Execute(PlayerCharacter* chara) = 0;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternNone
	@brief	なにもしないパターン
	@par	[説明]
		例：何も持っていないパターン
*//***************************************************************************/
class PlayerCharacterPatternNone : public PlayerCharacterPattern
{
public:
	PlayerCharacterPatternNone() {};
	virtual ~PlayerCharacterPatternNone() = default;

	virtual void Execute(PlayerCharacter* chara) {};
};

/**************************************************************************//**
	@class	PlayerCharacterPatternGender
	@brief	性別パターン
	@par	[説明]
		男性・女性の体・顔・モーションパーツ
*//***************************************************************************/
class PlayerCharacterPatternGender : public PlayerCharacterPattern
{
public:
	PlayerCharacterPatternGender(bool isMale) : PlayerCharacterPattern(), m_isMale(isMale) {}

	void Execute(PlayerCharacter* chara);
private:
	bool m_isMale;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternSingleModel
	@brief	単一モデル実装
	@par	[説明]
		単一のモデルを実装するパターン
*//***************************************************************************/
class PlayerCharacterPatternSingleModel : public PlayerCharacterPattern
{
public:
	PlayerCharacterPatternSingleModel(const char* filename) : PlayerCharacterPattern(), m_filename(filename) {}

	void Execute(PlayerCharacter* chara) override;
protected:
	std::string m_filename;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternSword
	@brief	剣モデル実装
	@par	[説明]
		剣モーション付きで剣を登録する
*//***************************************************************************/
class PlayerCharacterPatternSword : public PlayerCharacterPatternSingleModel
{
public:
	PlayerCharacterPatternSword(const char* filename) : PlayerCharacterPatternSingleModel(filename) {}

	void Execute(PlayerCharacter* chara) override;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternShield
	@brief	盾モデル実装
	@par	[説明]
		盾モーション付きで盾を登録する
*//***************************************************************************/
class PlayerCharacterPatternShield : public PlayerCharacterPatternSingleModel
{
public:
	PlayerCharacterPatternShield(const char* filename) : PlayerCharacterPatternSingleModel(filename) {}

	void Execute(PlayerCharacter* chara) override;
};

/**************************************************************************//**
	@class	PlayerCharacterPatternRod
	@brief	杖モデル実装
	@par	[説明]
		杖モーション付きで杖を登録する
*//***************************************************************************/
class PlayerCharacterPatternRod : public PlayerCharacterPatternSingleModel
{
public:
	PlayerCharacterPatternRod(const char* filename) : PlayerCharacterPatternSingleModel(filename) {}

	void Execute(PlayerCharacter* chara) override;
};

#endif //!__INCLUDED_PLAYER_CHARACTER_PATTERN_H__