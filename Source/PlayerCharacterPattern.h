//! @file PlayerCharacterPattern.h
//! @note 

#ifndef __INCLUDED_PLAYER_CHARACTER_PATTERN_H__
#define __INCLUDED_PLAYER_CHARACTER_PATTERN_H__

#include <string>

class PlayerCharacter;

/**************************************************************************//**
	@class	PlayerCharacterPattern
	@brief	�L�����N�^�[�p�^�[�����N���X
	@par	[����]
		�p�^�[���������ăv���C���[�L�����N�^�[��ݒ肷����N���X
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
	@brief	���ʂɂ���ĈႤ����
	@par	[����]

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
	@brief	�P�ꃂ�f�����������鏈��
	@par	[����]
		�ėp�^����
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