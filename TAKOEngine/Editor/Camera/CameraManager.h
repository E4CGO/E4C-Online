
//! @file CameraManager.h
//! @note カメラ管理

#ifndef __INCLUDED_TEST_BASE_H__
#define __INCLUDED_TEST_BASE_H__


#include<vector>
#include "TAKOEngine/Editor/Camera/Camera.h"
#include <set>


/**************************************************************************//**
		@class		CameraManager
		@brief	　　カメラ管理
		@par		カメラ生成・削除・更新

*//***************************************************************************/
class CameraManager
{
private:
	//!コンストラクタ
	CameraManager() {}
	//!デストラクタ
	~CameraManager() {}
public:
	//唯一のインスタンス取得
	static CameraManager& Instance()
	{
		static CameraManager instance;
		return instance;
	}
	//!更新
	void Update();
	//!カメラの数
	int GetCameraCount() const { return static_cast<int>(cameras.size()); }

	// 現在カメラを設定
	void SetCamera(int id) { m_id = id; }
	//!現在カメラ取得
	Camera* GetCamera(void) { return cameras.at(m_id); }
	//!カメラ取得
	Camera* GetCamera(int index) { return cameras.at(index); }
	//!登録
	void Register(Camera* camera);
	//!クリア
	void Clear();
	//!削除
	void Remove(Camera* camera);

private:
	int m_id = -1;
	std::vector<Camera*> cameras;
	std::set<Camera*> removes;
};
#endif	// __INCLUDED_TEST_BASE_H__