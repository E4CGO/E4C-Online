//! @file SpawnerManager.cpp
//! @note 

#include "SpawnerManager.h"

/**************************************************************************//**
 	@brief		スポナーのインデックスを取り出す
	@param[in]	spawner スポナーの参照ポインタ
	@return		スポナーインデックス、見つからない場合はUINT8_MAX
*//***************************************************************************/
const uint8_t SpawnerManager::GetIndex(Spawner* spawner)
{
	uint8_t result = 0;
	for (Spawner* item : items)
	{
		if (item == spawner)
		{
			return result;
		}
		result++;
	}
	return UINT8_MAX;
}