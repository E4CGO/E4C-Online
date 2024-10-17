#include "Map/SimpleRoom1.h"

SimpleRoom1::SimpleRoom1(RoomBase* parent, DirectX::XMFLOAT3 moveValue) :
	RoomBase(parent, moveValue)
{
	m_startPos = { 0.0f, 0.0f, 0.0f };
	m_endPos = { 0.0f, 0.0f, 4.0f };
	m_nextPos = { 0.0f, 0.0f, 8.0f };

	m_startPos += moveValue;
	m_endPos += moveValue;
	m_nextPos += moveValue;

	PlaceMapTile();

	if (GetDepth() < 3)
	{
		// 部屋の始点と終点から次の部屋の移動値を求める
		//DirectX::XMFLOAT3 nextMoveValue = (m_endPos - m_startPos);

		//SimpleRoom1* nextRoom = new SimpleRoom1(this, nextMoveValue);
		SimpleRoom1* nextRoom = new SimpleRoom1(this, m_nextPos);
		AddRoom(nextRoom);
	}
}

void SimpleRoom1::PlaceMapTile()
{
	MapTile* startTile = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
	startTile->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	startTile->SetPosition(m_startPos);
	//startTile->SetParent(parent);
	MAPTILES.Register(startTile);

	MapTile* endTile = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
	endTile->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	endTile->SetPosition(m_endPos);
	//endTile->SetParent(parent);
	MAPTILES.Register(endTile);
}