#include "Mage.h"

Mage::Mage() : Player("Data/Model/Character/Mage.glb", 1.0f)
{
	hp = maxHp = 100;
	type = PLAYER_CLASS::MAGE;

	model->FindNode("1H_Wand")->visible = false;
	model->FindNode("Spellbook")->visible = false;
	model->FindNode("Spellbook_open")->visible = false;
}