#pragma once

#include <memory>

#include "TAKOEngine/GUI/Widget.h"

#include "GameObject/Character/Player/Player.h"

class WidgetSkillTimer : public Widget
{
public:
	WidgetSkillTimer();

	void Render(const RenderContext& rc);
private:
	enum ICON {
		ICON_ROCK,			// スキルアイコン_岩.png
		ICON_FLAG,			// スキルアイコン_旗.png
		ICON_HAND,			// スキルアイコン_手.png
		ICON_BOOK,			// スキルアイコン_本.png
		ICON_ARROW,			// スキルアイコン_矢.png
		ICON_WING,			// スキルアイコン_翼.png
		ICON_BUG,			// スキルアイコン_バグ.png
		ICON_BUFF,			// スキルアイコン_バフ.png
		ICON_TALK,			// スキルアイコン_フキダシ.png

		ICON_FEATHER,		// スキルアイコン_羽根.png
		ICON_GEAR,			// スキルアイコン_歯車.png
		ICON_HEAL,			// スキルアイコン_治療.png
		ICON_EXPERIMENT,	// スキルアイコン_実験.png
		ICON_IMPACT,		// スキルアイコン_衝撃.png
		ICON_SNIPER,		// スキルアイコン_狙撃.png
		ICON_NINJA,			// スキルアイコン_忍術.png
		ICON_BOMB,			// スキルアイコン_爆弾.png
		ICON_RELOAD,		// スキルアイコン_リロード.png

		ICON_BLOCK,			// スキルアイコン_防御.png
		ICON_NONE,			// スキルアイコン_無地.png
		ICON_SLASH_1,		// スキルアイコン_斬撃1.png
		ICON_MAGIC_1,		// スキルアイコン_魔法1.png
		ICON_MAGIC_10,		// スキルアイコン_魔法10.png
		ICON_MAGIC_11,		// スキルアイコン_魔法11.png
		ICON_MAGIC_12,		// スキルアイコン_魔法12.png
		ICON_MAGIC_13,		// スキルアイコン_魔法13.png
		ICON_CC,			// スキルアイコン_状態異常.png

		ICON_MAGIC_14,		// スキルアイコン_魔法14.png
		ICON_SLASH_2,		// スキルアイコン_斬撃2.png
		ICON_MAGIC_2,		// スキルアイコン_魔法2.png
		ICON_SLASH_3,		// スキルアイコン_斬撃3.png
		ICON_MAGIC_3,		// スキルアイコン_魔法3.png
		ICON_SLASH_4,		// スキルアイコン_斬撃4.png
		ICON_MAGIC_4,		// スキルアイコン_魔法4.png
		ICON_SLASH_5,		// スキルアイコン_斬撃5.png
		ICON_AIR_ATTACK,	// スキルアイコン_対空攻撃.png

		ICON_MAGIC_5,		// スキルアイコン_魔法5.png
		ICON_SLASH_6,		// スキルアイコン_斬撃6.png
		ICON_MAGIC_6,		// スキルアイコン_魔法6.png
		ICON_MAGIC_7,		// スキルアイコン_魔法7.png
		ICON_MAGIC_8,		// スキルアイコン_魔法8.png
		ICON_MAGIC_9,		// スキルアイコン_魔法9.png
		ICON_MIKO,			// スキルアイコン_お祓い.png
		ICON_DEBUFF,		// スキルアイコン_デバフ.png
		ICON_SCYTHE,		// スキルアイコン_大鎌攻撃.png

		ICON_SKULL,			// スキルアイコン_ドクロ.png
		ICON_PUNCH,			// スキルアイコン_パンチ.png
		ICON_BREATH,		// スキルアイコン_ブレス.png
		ICON_RING,			// スキルアイコン_リング.png
		ICON_HEALER,		// スキルアイコン_回復役.png
		ICON_BOW,			// スキルアイコン_弓攻撃.png
		ICON_SWORD,			// スキルアイコン_剣攻撃.png
		ICON_RRIDENT,		// スキルアイコン_三叉槍.png
		ICON_CLAYMORE,		// スキルアイコン_大剣攻撃.png

		ICON_GUN,			// スキルアイコン_銃攻撃.png
		ICON_LAMP,			// スキルアイコン_照らす.png
		ICON_HAMMER,		// スキルアイコン_槌攻撃.png
		ICON_KATANA,		// スキルアイコン_刀攻撃.png
		ICON_AXE,			// スキルアイコン_斧攻撃.png
		ICON_ITEM_BAG,		// スキルアイコン_道具袋.png
		ICON_SPECIAL_1,		// スキルアイコン_必殺技1.png
		ICON_SPECIAL_10,	// スキルアイコン_必殺技10.png
		ICON_SUMMON,		// スキルアイコン_魔物召喚.png

		ICON_SPECIAL_2,		// スキルアイコン_必殺技2.png
		ICON_SPECIAL_3,		// スキルアイコン_必殺技3.png
		ICON_SPECIAL_4,		// スキルアイコン_必殺技4.png
		ICON_SPECIAL_5,		// スキルアイコン_必殺技5.png
		ICON_SPECIAL_6,		// スキルアイコン_必殺技6.png
		ICON_SPECIAL_7,		// スキルアイコン_必殺技7.png
		ICON_SPECIAL_8,		// スキルアイコン_必殺技8.png
		ICON_SPECIAL_9,		// スキルアイコン_必殺技9.png
		ICON_MAGIC_EFFECT,	// スキルアイコン_魔法効果.png

		ICON_MAGIC_ATTACK,	// スキルアイコン_魔法攻撃.png
		ICON_COUNTER,		// スキルアイコン_カウンター.png
		ICON_KNIFE,			// スキルアイコン_ナイフ攻撃.png
		ICON_LOCKON,		// スキルアイコン_ロックオン.png
		ICON_HARISEN,		// スキルアイコン_ハリセン攻撃.png
		ICON_HALBERD,		// スキルアイコン_ハルバード攻撃.png
	};
	const int lineBreak = 9;

private:
	void DrawIcon(
		const RenderContext& rc,
		const int& imageIdx,
		const DirectX::XMFLOAT2& pos,
		int skillTimerIdx,
		const char* key
	);
private:
	Player* player = nullptr;
	std::shared_ptr<Sprite> skillIcons;
	std::shared_ptr<Sprite> mask;
};
