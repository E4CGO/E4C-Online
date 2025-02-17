#include <vector>
#include <mutex>
#include <d3d12.h>
#include <wrl.h>

//============================================================================
// コマンドリスト
//----------------------------------------------------------------------------
class CommandList
{
public:
	CommandList(ID3D12Device* d3d_device, D3D12_COMMAND_LIST_TYPE d3d_command_list_type);
	~CommandList();

	// 開始処理
	void Begin();

	// 終了処理
	void End();

	// コマンドリスト取得
	ID3D12GraphicsCommandList* GetCommandList() const { return m_d3d_command_list.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_d3d_command_allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_d3d_command_list;
};