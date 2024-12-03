#include "Mage.h"

Mage::Mage() : Player("Data/Model/Character/Mage.glb", 1.0f)
{
	hp = maxHp = 100;
	type = PLAYER_CLASS::MAGE;

	m_pmodels[0]->FindNode("1H_Wand")->visible = false;
	m_pmodels[0]->FindNode("Spellbook")->visible = false;
	m_pmodels[0]->FindNode("Spellbook_open")->visible = false;
}