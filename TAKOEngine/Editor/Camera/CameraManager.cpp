//! @file CameraManager.cpp
//! @note カメラ管理
#include "TAKOEngine/Editor/Camera/CameraManager.h"
/**************************************************************************//**
		@brief		更新
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void CameraManager::Update()
{

	//破棄処理
	for (Camera* camera : removes)
	{
		auto it = std::find(cameras.begin(), cameras.end(), camera);

		if (it != cameras.end())
		{
			cameras.erase(it);
		}
		delete camera;
	}
	removes.clear();

}
/**************************************************************************//**
		@brief		削除
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void CameraManager::Remove(Camera* camera)
{
	//破棄リストに追加
	removes.insert(camera);
}
/**************************************************************************//**
		@brief		登録
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void CameraManager::Register(Camera* camera)
{
	cameras.emplace_back(camera);
}
/**************************************************************************//**
		@brief		クリア
		@param[in]	なし
		@return		なし
*//***************************************************************************/
void CameraManager::Clear()
{
	for (Camera* camera : cameras)
	{
		delete camera;
	}
	cameras.clear();
}