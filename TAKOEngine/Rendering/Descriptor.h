#pragma once

#pragma once

#include <vector>
#include <mutex>
#include <d3d12.h>
#include <wrl.h>

//============================================================================
// �f�B�X�N���v�^
//----------------------------------------------------------------------------
class Descriptor
{
public:
	// ������
	void Initialize(D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_descriptor_handle, D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_descriptor_handle, UINT index);

	// GPU�n���h���擾
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_d3d_cpu_descriptor_handle; }

	// GPU�n���h���擾
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return m_d3d_gpu_descriptor_handle; }

	// �f�B�X�N���v�^�q�[�v���̃C���f�b�N�X�擾
	UINT GetIndex() const { return m_index; }

private:
	D3D12_CPU_DESCRIPTOR_HANDLE	m_d3d_cpu_descriptor_handle = { 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3d_gpu_descriptor_handle = { 0 };
	UINT						m_index = 0;
};

//============================================================================
// �f�B�X�N���v�^�q�[�v
//----------------------------------------------------------------------------
class DescriptorHeap
{
public:
	DescriptorHeap(ID3D12Device* d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE d3d_descriptor_heap_type, UINT max_descriptor_count);
	~DescriptorHeap();

public:
	// D3D�f�B�X�N���v�^�q�[�v�擾
	ID3D12DescriptorHeap* GetD3DDescriptorHeap() const { return m_d3d_descriptor_heap.Get(); }

	// �f�B�X�N���v�^���󂯎��
	Descriptor* PopDescriptor();

	// �f�B�X�N���v�^��ԋp����
	void PushDescriptor(Descriptor* descriptor);

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_d3d_descriptor_heap;
	std::mutex											m_mutex;
	std::vector<Descriptor>								m_descriptors;
	std::vector<Descriptor*>							m_descriptor_stack;
	int													m_descriptor_stack_index = 0;
};
