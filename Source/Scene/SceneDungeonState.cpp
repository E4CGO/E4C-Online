#include "SceneDungeonState.h"

#include "profiler.h"

#include "TAKOEngine/GUI/UIManager.h"
#include "TAKOEngine/Editor/Camera/ThridPersonCameraController.h"

#include "GameObject/Character/Player\PlayerManager.h"

#include "UI/Widget/WidgetCrosshair.h"
#include "UI/Widget/WidgetPlayerHP.h"
#include "UI/Widget/WidgetEnemiesHp.h"
#include "UI/Widget/WidgetSkillTimer.h"

#include "Scene/SceneManager.h"
#include "Scene/SceneTitle.h"

#include "GameData.h"



//// �R���X�g���N�^
//SceneDungeonState::Room::Room(RoomDataBase roomData, Room* parent)
//{
//	this->roomData = roomData;
//	this->parent = parent;
//
//	// �e���猩���[�x�������󂢂Ȃ�q�𐶐�����
//	if (GetDepth() < 4)
//	{
//		// �����̏I�_�Ǝn�_����ړ��l�����߂�
//		DirectX::XMFLOAT3 movePos = (this->roomData.GetEndPos() - this->roomData.GetStartPos());
//		RoomData::DefaultRoom newRoom(this->roomData.GetNextPos());
//
//		AddRoom(newRoom);
//	}
//}

//void SceneDungeonState::Room::Update(float elapsedTime)
//{
//	if (child.size() == 0)
//	{
//		return;
//	}
//
//	roomData.Update(elapsedTime);
//
//	for (Room* room : child)
//	{
//		room->Update(elapsedTime);
//	}
//}

// �f�X�g���N�^
SceneDungeonState::GenerateState::~GenerateState()
{
	delete rootRoom;
}

// �����X�e�[�g
void SceneDungeonState::GenerateState::Enter()
{
	rootRoom = new SimpleRoom1();
}

void SceneDungeonState::GenerateState::Execute(float elapsedTime)
{
	auto& a = MAPTILES.GetAll();

	//rootRoom->Update(elapsedTime);
}

void SceneDungeonState::GenerateState::Exit()
{
}

void SceneDungeonState::GenerateState::CreateRoom()
{
	//DungeonRoom::DefaultRoom newRoom;

	//rootRoom = new Room(&newRoom, nullptr);

	//for (MapTile& tile : rootRoom->GetMapTiles())
	//{
	//	MAPTILES.Register(&tile);
	//}
}

// �����ݒ�����Ƀ}�b�v�f�[�^�̏��������s��
//void SceneDungeonState::GenerateState::InitializeMapData()
//{
//	// �_���W�����̑傫����ݒ肵���傫���ɂ���
//	m_Map.resize(m_DungeonSettings.maxMapWidth);
//	for (int x = 0; x < m_Map.size(); x++) m_Map[x].resize(m_DungeonSettings.maxMapHeight);
//
//	// �_���W�����̑S�Ẵ^�C�����u�Ȃɂ��Ȃ��v�ɂ���
//	for (int x = 0; x < m_Map.size(); x++) for (int y = 0; y < m_Map.size(); y++) m_Map[x][y] = FloorType::NONE;
//}

// ���W�̎w�肵�����p�����ł��邩�ǂ����𒲂ׂ�
//bool SceneDungeonState::GenerateState::IsCorner(MAP_COORD coord, Direction direction)
//{
//	switch (direction) {
//	case SceneDungeonState::LEFT:
//		if (coord.x <= 0) return true;
//		break;
//	case SceneDungeonState::RIGHT:
//		if (coord.x >= (m_DungeonSettings.maxMapWidth - 1)) return true;
//		break;
//	case SceneDungeonState::TOP:
//		if (coord.y <= 0) return true;
//		break;
//	case SceneDungeonState::BOTTOM:
//		if (coord.y >= (m_DungeonSettings.maxMapHeight - 1)) return true;
//		break;
//	default:
//		break;
//	}
//
//	return false;
//}

// �����̎w�肵�����p�����ł��邩�ǂ����𒲂ׂ�
//bool SceneDungeonState::GenerateState::IsCorner(ROOM_DATA roomData, Direction direction)
//{
//	switch (direction) {
//	case SceneDungeonState::LEFT:
//		if (roomData.left <= 0) return true;
//		break;
//	case SceneDungeonState::RIGHT:
//		if (roomData.right >= (m_DungeonSettings.maxMapWidth - 1)) return true;
//		break;
//	case SceneDungeonState::TOP:
//		if (roomData.top <= 0) return true;
//		break;
//	case SceneDungeonState::BOTTOM:
//		if (roomData.bottom >= (m_DungeonSettings.maxMapHeight - 1)) return true;
//		break;
//	default:
//		break;
//	}
//
//	return false;
//}

// �ڑ��_���畔���������ł��邩�𒲂ׂ�
//bool SceneDungeonState::GenerateState::CheckCreateNewRoom(
//	CONNECTION_POINT connectionPoint,
//	ROOM_DATA& maxRoomData)
//{
//	maxRoomData.left = maxRoomData.right = connectionPoint.coord.x;
//	maxRoomData.top = maxRoomData.bottom = connectionPoint.coord.x;
//
//	// �����T��
//	// �������[�Ȃ�T�����s��Ȃ�
//	if (!IsCorner(connectionPoint.coord, Direction::LEFT)) {
//		for (int x = connectionPoint.coord.x; x > 0; x--) {
//			if (m_Map[x - 1][connectionPoint.coord.y] != FloorType::NONE) {
//				maxRoomData.left = x;
//				break;
//			}
//		}
//	}
//
//	// �E���T��
//	// �����E�[�Ȃ�T�����s��Ȃ�
//	if (!IsCorner(connectionPoint.coord, Direction::RIGHT)) {
//		for (int x = connectionPoint.coord.x; x < (m_DungeonSettings.maxMapWidth - 1); x++) {
//			if (m_Map[x + 1][connectionPoint.coord.y] != FloorType::NONE) {
//				maxRoomData.right = x;
//				break;
//			}
//		}
//	}
//
//	// �㑤�T��
//	// ������[�Ȃ�T�����s��Ȃ�
//	if (!IsCorner(connectionPoint.coord, Direction::TOP)) {
//		for (int y = connectionPoint.coord.y; y > 0; y--) {
//			if (m_Map[connectionPoint.coord.x][y - 1] != FloorType::NONE) {
//				maxRoomData.top = y;
//				break;
//			}
//		}
//	}
//
//	// �����T��
//	// �������[�Ȃ�T�����s��Ȃ�
//	if (!IsCorner(connectionPoint.coord, Direction::BOTTOM)) {
//		for (int y = connectionPoint.coord.y; y < (m_DungeonSettings.maxMapHeight - 1); y++) {
//			if (m_Map[connectionPoint.coord.x][y + 1] != FloorType::NONE) {
//				maxRoomData.bottom = y;
//				break;
//			}
//		}
//	}
//
//	// �����������ł��邩���ׂ�
//	int roomWidth = (maxRoomData.right - maxRoomData.left) + 1;
//	int roomHeight = (maxRoomData.bottom - maxRoomData.top) + 1;
//
//	// �ő�T�C�Y�ȉ����ŏ��T�C�Y�ȏ�Ȃ琶���ł��邽�� true ��Ԃ�
//	if (roomWidth <= m_DungeonSettings.maxRoomWidth &&
//		roomWidth >= m_DungeonSettings.minRoomWidth &&
//		roomHeight <= m_DungeonSettings.maxRoomHeight &&
//		roomHeight >= m_DungeonSettings.minRoomHeight) return true;
//
//	return false;
//}

// �����ɐڑ��_��ݒ肷��
//void SceneDungeonState::GenerateState::AddConnectionPoint(ROOM_DATA data)
//{
//
//}

// �������_���W�����^�C���z��ɏ�������
//void SceneDungeonState::GenerateState::WriteRoomData(ROOM_DATA data)
//{
//	for (int x = data.left; x <= data.right; x++) {
//		for (int y = data.top; y <= data.bottom; y++) {
//			m_Map[x][y] = FloorType::FLOOR;
//		}
//	}
//}

// �����𐶐�����
//void SceneDungeonState::GenerateState::CreateRoom()
//{
//	ROOM_DATA firstRoomData;
//	firstRoomData.left = 0;
//	firstRoomData.right = 15;
//	firstRoomData.top = 0;
//	firstRoomData.bottom = 2;
//
//	rootRoom = new Room(firstRoomData, nullptr);
//	WriteRoomData(firstRoomData);
//
//	ROOM_DATA secondRoomData;
//	secondRoomData.left = 7;
//	secondRoomData.right = 8;
//	secondRoomData.top = 3;
//	secondRoomData.bottom = 15;
//
//	rootRoom->AddRoom(secondRoomData);
//	WriteRoomData(secondRoomData);
//
//
//
//	PlaceRoom(firstRoomData);
//	PlaceRoom(secondRoomData);
//
//	//ROOM newRoom;
//
//	//newRoom.data = roomData;
//
//	//newRoom.parent = nullptr;
//
//	//m_RoomDatas.emplace_back(newRoom);
//
//	//// �����Ń^�C���ۑ��p�z��ɏ�������
//	//for (int x = roomData.left; x <= roomData.right; x++) {
//	//	for (int y = roomData.top; y <= roomData.bottom; y++) {
//	//		m_Map[x][y] = FloorType::FLOOR;
//	//	}
//	//}
//}

//// �ڑ��_���畔���𐶐�����
//SceneDungeonState::ROOM* SceneDungeonState::GenerateState::CreateRoom(
//	CONNECTION_POINT connectionPoint,
//	ROOM* parent)
//{
//	ROOM* newRoom = new ROOM();
//
//	parent->child.emplace_back(newRoom);
//
//	return newRoom;
//}

// �����f�[�^�����Ƀ��f����z�u����
//void SceneDungeonState::GenerateState::PlaceRoom(ROOM_DATA roomData)
//{
//	for (int x = roomData.left; x <= roomData.right; x++) {
//		for (int y = roomData.top; y <= roomData.bottom; y++) {
//			MapTile* tile = new MapTile("Data/Model/Dungeon/Floor_Plain_Parent.glb", 1.0f);
//			tile->SetPosition({ ((float)x * m_TileScale), 0.0f, ((float)y * m_TileScale) });
//
//			float colorR = (1.0f / m_DungeonSettings.maxMapWidth) * x;
//			float colorG = (1.0f / m_DungeonSettings.maxMapHeight) * y;
//
//			tile->SetColor({ colorR, colorG, 0.0f, 1.0f });
//			MAPTILES.Register(tile);
//		}
//	}
//}

// �Q�[���X�e�[�g
void SceneDungeonState::GameState::Enter()
{

}

void SceneDungeonState::GameState::Execute(float elapsedTime)
{

}

void SceneDungeonState::GameState::Exit()
{

}