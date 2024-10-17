#pragma once

#include "TAKOEngine/AI/BaseState.h"

#include <vector>

#include "Map/MapTileManager.h"
#include "Map/MapTile.h"

#include "Map/RoomBase.h"
#include "Map/SimpleRoom1.h"

#include "Scene/SceneDungeon.h"
#include "UI/Widget/WidgetText.h"

namespace SceneDungeonState
{
	enum DungeonState
	{
		GENERATE = 0,
		GAME,
	};

	// ����
	enum Direction
	{
		LEFT = 0,
		RIGHT,
		TOP,
		BOTTOM,
	};

	// �_���W���������X�e�[�g
	class GenerateState : public HierarchicalState<SceneDungeon>
	{
	public:
		// �R���X�g���N�^
		GenerateState(SceneDungeon* scene) : HierarchicalState<SceneDungeon>(scene) {};

		// �f�X�g���N�^
		~GenerateState();

		// �X�e�[�g�ɓ��������̃��\�b�h
		virtual void Enter() override;

		// �X�e�[�g�Ŏ��s���郁�\�b�h
		void Execute(float elapsedTime) override;

		// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
		void Exit() override;

		// �����ݒ�̃Q�b�^�[�A�Z�b�^�[
		//DUNGEON_SETTINGS GetDungeonSettings() { return m_DungeonSettings; }
		//void SetDungeonSettings(DUNGEON_SETTINGS newSetting) { m_DungeonSettings = newSetting; }

	private:
		// �����ݒ�����Ƀ}�b�v�f�[�^�̏��������s��
		//void InitializeMapData();

		// ���W�̎w�肵�����p�����ł��邩�ǂ����𒲂ׂ�
		//bool IsCorner(MAP_COORD coord, Direction direction);

		// �����̎w�肵�����p�����ł��邩�ǂ����𒲂ׂ�
		//bool IsCorner(ROOM_DATA roomData, Direction direction);

		// �ڑ��_�f�[�^���畔���𐶐��ł��邩�𒲂ׁA
		// �����ł���ő�T�C�Y�̕����f�[�^���Z�o����
		//bool CheckCreateNewRoom(
		//	CONNECTION_POINT connectionPoint,
		//	ROOM_DATA& maxRoomData);

		// �������_���W�����^�C���z��ɏ�������
		//void WriteRoomData(ROOM_DATA data);

		// �����ɐڑ��_��ǉ�����
		//void AddConnectionPoint(ROOM_DATA data);

		// �����𐶐�����
		void CreateRoom();

		// �����f�[�^�����Ƀ��f����z�u����
		//void PlaceRoom(ROOM_DATA roomData);



		//int m_TileScale = 4.0f;	// �^�C���̃X�P�[��

		RoomBase* rootRoom;	// �ŏ��̕���

		//DUNGEON_SETTINGS m_DungeonSettings;			// �_���W�����̐����ݒ�
		//std::vector<std::vector<FloorType>> m_Map;	// �_���W�����^�C���ۑ��p�z��
	};

	// �Q�[���X�e�[�g
	class GameState : public HierarchicalState<SceneDungeon>
	{
	public:
		// �R���X�g���N�^
		GameState(SceneDungeon* scene) : HierarchicalState<SceneDungeon>(scene) {};

		// �f�X�g���N�^
		~GameState() {}

		// �X�e�[�g�ɓ��������̃��\�b�h
		virtual void Enter() override;

		// �X�e�[�g�Ŏ��s���郁�\�b�h
		void Execute(float elapsedTime) override;

		// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
		void Exit() override;
	};
}