
//! @file CameraManager.h
//! @note �J�����Ǘ�

#ifndef __INCLUDED_TEST_BASE_H__
#define __INCLUDED_TEST_BASE_H__


#include<vector>
#include "TAKOEngine/Editor/Camera/Camera.h"
#include <set>


/**************************************************************************//**
		@class		CameraManager
		@brief	�@�@�J�����Ǘ�
		@par		�J���������E�폜�E�X�V

*//***************************************************************************/
class CameraManager
{
private:
	//!�R���X�g���N�^
	CameraManager() {}
	//!�f�X�g���N�^
	~CameraManager() {}
public:
	//�B��̃C���X�^���X�擾
	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}
	//!�X�V
	void Update();
	//!�J�����̐�
	int GetCameraCount() const { return static_cast<int>(cameras.size()); }

	// ���݃J������ݒ�
	void SetCamera(int id) { m_id = id; }
	//!���݃J�����擾
	Camera* GetCamera(void) { return cameras.at(m_id); }
	//!�J�����擾
	Camera* GetCamera(int index) { return cameras.at(index); }
	//!�o�^
	void Register(Camera* camera);
	//!�N���A
	void Clear();
	//!�폜
	void Remove(Camera* camera);

private:
	int m_id = -1;
	std::vector<Camera*> cameras;
	std::set<Camera*> removes;
};
#endif	// __INCLUDED_TEST_BASE_H__