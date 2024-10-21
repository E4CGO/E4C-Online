#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"
#include "Scene/GameLoop/SceneTitle/SceneTitle_E4C.h"
#include "Scene/GameLoop/SceneCharacter/SceneCharacter_E4C.h"
#include "Scene/GameLoop/SceneGame/SceneGame_E4C.h"

#include "TAKOEngine/AI/StateMachine.h"

#include "GameObject/Character/Player/Player.h"
#include "GameObject/Character/Enemy/Enemy.h"

template <typename T>
StateMachine<T>::~StateMachine()
{
	for (std::pair<int, State<T>*> state : statePool)
	{
		delete state.second;
	}
	statePool.clear();
}
// �X�V����
template <typename T>
void StateMachine<T>::Update(float elapsedTime)
{
	currentState->Execute(elapsedTime);
}
// �X�e�[�g�Z�b�g
template <typename T>
void StateMachine<T>::SetState(int newState)
{
	if (statePool.find(newState) == statePool.end()) return;
	currentState = statePool[newState];
	currentState->Enter();
}
// �X�e�[�g�ύX
template <typename T>
void StateMachine<T>::ChangeState(int newState)
{
	if (statePool.find(newState) == statePool.end()) return;
	// ���݂̃X�e�[�g��Exit�֐������s�A�V�����X�e�[�g���Z�b�g�A�V�����X�e�[�g��Enter�֐����Ăяo���B
	currentState->Exit();

	SetState(newState);
}
// �X�e�[�g�o�^
template <typename T>
void StateMachine<T>::RegisterState(int id, HierarchicalState<T>* state)
{
	if (statePool.find(id) != statePool.end()) delete statePool[id];
	// �e�X�e�[�g�o�^
	statePool[id] = state;
}
// �X�e�[�g�ԍ��擾
template <typename T>
int StateMachine<T>::GetStateIndex()
{
	for (std::pair<int, State<T>*> state : statePool)
	{
		if (state.second == currentState)
		{
			// i�ԍ��ڂ̃X�e�[�g�����^�[��
			return state.first;
		}
	}

	// �X�e�[�g��������Ȃ�������
	return -1;
}

// �Q�w�ڃX�e�[�g�ύX
template <typename T>
void StateMachine<T>::ChangeSubState(int newState)
{
	// HierarchicalState�N���X��ChangeSubState���Ăяo��
	currentState->ChangeSubState(newState);
}
// �Q�w�ڃX�e�[�g�o�^
template <typename T>
void StateMachine<T>::RegisterSubState(int index, int subIndex, State<T>* subState)
{
	if (statePool.find(index) == statePool.end()) return; // ���݂��Ȃ�
	statePool[index]->RegisterSubState(subIndex, subState);
}

template class StateMachine<Player>;
template class StateMachine<Enemy>;
template class StateMachine<SceneTitle>;
template class StateMachine<SceneGame>;
template class StateMachine<SceneTitle_E4C>;
template class StateMachine<SceneCharacter_E4C>;
template class StateMachine<SceneGame_E4C>;