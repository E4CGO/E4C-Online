#include "EventOpening.h"

#include "DialogEvent.h"
#include "CharacterMoveEvent.h"
#include "FlagEvent.h"

EventOpening::EventOpening(PlayableCharacter* mario, PlayableCharacter* luig)
{
	this->mario = mario;
	this->luig = luig;
	actions.emplace_back(new CharacterVisible(mario, false));
	actions.emplace_back(new CharacterPosition(luig, { 500.0f, 31.0f, -72.131f }));
	actions.emplace_back(new CharacterRotate(luig, { 0.0f, 0.0f, 0.0f }));
	actions.emplace_back(new CharacterPosition(mario, { 98.0f, 31.0f, -72.131f }));
	actions.emplace_back(new CharacterRotate(mario, { 0.0f, DirectX::XMConvertToRadians(90), 0.0f }));

	actions.emplace_back(new DialogEvent("", "たのも――――――――――――――――――――――――！"));
	actions.emplace_back(new CharacterJump(luig, { 0.0f, 500.0f, 0.0f }));
	actions.emplace_back(new LuigDialogEvent(luig, "！？"));
	actions.emplace_back(new CharacterTurn(luig, { -1.0f, 0.0f, 0.0f }));
	actions.emplace_back(new DialogHide);
	actions.emplace_back(new CharacterVisible(mario, true));
	actions.emplace_back(new CharacterMove(mario, { 400.0f, 31.0f, -72.131f }));

	actions.emplace_back(new LuigDialogEvent(luig, "お姉さま！？"));
	actions.emplace_back(new MarioDialogEvent(mario, "ルイ！大変だ！家の前にモンスターが！"));
	actions.emplace_back(new LuigDialogEvent(luig, "展開早い過ぎませんか！？"));
	actions.emplace_back(new MarioDialogEvent(mario, "締め切り一日前だからストーリー入れるわけがないだろ！"));
	actions.emplace_back(new LuigDialogEvent(luig, "メタタタタタ...."));
	actions.emplace_back(new MarioDialogEvent(mario, "誰だよ一ヵ月でRPG作ろうとするヤツは！"));
	actions.emplace_back(new LuigDialogEvent(luig, "作者の自業自得じゃないですか..."));
	actions.emplace_back(new MarioDialogEvent(mario, "とにかく早くモンスター倒しに行こうよ！今それしかないんだから！(ゲーム的に)"));
	actions.emplace_back(new LuigDialogEvent(luig, "チュートリアルは？"));
	actions.emplace_back(new MarioDialogEvent(mario, "ないよ！まんまマリ〇RPGと言ったら分かるだろう！"));
	actions.emplace_back(new LuigDialogEvent(luig, "こんな適当な..."));
	actions.emplace_back(new MarioDialogEvent(mario, "嫌ならルイがやってよ。"));
	actions.emplace_back(new LuigDialogEvent(luig, "えっと..."));
	actions.emplace_back(new LuigDialogEvent(luig, "方向キーは移動、A、Bボタンはジャンプ、Shiftで交代"));
	actions.emplace_back(new MarioDialogEvent(mario, "マリ〇 RPGと一緒じゃん"));
	actions.emplace_back(new LuigDialogEvent(luig, "お姉さまうるさいです。"));
	actions.emplace_back(new CharacterJump(mario, { 0.0f, 500.0f, 0.0f }));
	actions.emplace_back(new MarioDialogEvent(mario, "説明必要ほどコンテンツ入ってないからもう行こう！"));
	actions.emplace_back(new LuigDialogEvent(luig, "しょうがないですね..."));

	actions.emplace_back(new DialogHide);
	actions.emplace_back(new FlagEvent("flags_opening_movie", true));
}

void EventOpening::Update(float elapsedTime)
{
	EventScene::Update(elapsedTime);

	// キャラ更新
	mario->Update(elapsedTime);
	luig->Update(elapsedTime);
}