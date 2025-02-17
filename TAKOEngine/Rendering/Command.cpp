#include "Command.h"
#include "Misc.h"
#include <crtdbg.h>

//============================================================================
// コマンドリスト
//----------------------------------------------------------------------------
// コンストラクタ
CommandList::CommandList(ID3D12Device* d3d_device, D3D12_COMMAND_LIST_TYPE d3d_command_list_type)
{
	HRESULT hr;

	hr = d3d_device->CreateCommandAllocator(
		d3d_command_list_type,											// コマンドアロケータの種類
		IID_PPV_ARGS(m_d3d_command_allocator.GetAddressOf())			// 格納するポインタのアドレス
	);
	COMPLETION_CHECK
		m_d3d_command_allocator->SetName(L"CommandAllocator");

	hr = d3d_device->CreateCommandList(
		0,
		d3d_command_list_type,
		m_d3d_command_allocator.Get(),
		nullptr,
		IID_PPV_ARGS(m_d3d_command_list.GetAddressOf())
	);
	COMPLETION_CHECK
		m_d3d_command_list->Close();
	m_d3d_command_list->SetName(L"CommandList");
}

// デストラクタ
CommandList::~CommandList()
{
}

// コマンド積み込み開始
void CommandList::Begin()
{
	m_d3d_command_allocator->Reset();
	m_d3d_command_list->Reset(m_d3d_command_allocator.Get(), nullptr);
}

// コマンド積み込み終了
void CommandList::End()
{
	m_d3d_command_list->Close();
}