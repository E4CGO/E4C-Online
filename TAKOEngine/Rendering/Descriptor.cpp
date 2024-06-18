#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Descriptor.h"
#include "TAKOEngine/Tool/Logger.h"

//============================================================================
// �f�B�X�N���v�^
//----------------------------------------------------------------------------
// ������
void Descriptor::Initialize(D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_descriptor_handle, D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_descriptor_handle, UINT index)
{
	m_d3d_cpu_descriptor_handle = d3d_cpu_descriptor_handle;
	m_d3d_gpu_descriptor_handle = d3d_gpu_descriptor_handle;
	m_index = index;
}

//============================================================================
// �f�B�X�N���v�^�q�[�v����I�u�W�F�N�g
//----------------------------------------------------------------------------
// �R���X�g���N�^
DescriptorHeap::DescriptorHeap(ID3D12Device* d3d_device, D3D12_DESCRIPTOR_HEAP_TYPE d3d_descriptor_heap_type, UINT max_descriptor_count)
{
	HRESULT hr;

	// �f�B�X�N���v�^�q�[�v�^�C�v���̐ݒ�
	LPCWSTR name = L"";
	D3D12_DESCRIPTOR_HEAP_FLAGS d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	switch (d3d_descriptor_heap_type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		name = L"CBV_SRV_UAV_DescriptorHeap";
		d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		name = L"Sampler_DescriptorHeap";
		d3d_descriptor_heap_flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		name = L"RTV_DescriptorHeap";
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		name = L"DSV_DescriptorHeap";
		break;
	}

	// �f�B�X�N���v�^�q�[�v����
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = d3d_descriptor_heap_type;					// �f�B�X�N���v�^�q�[�v�̃^�C�v
	desc.NumDescriptors = max_descriptor_count;				// �i�[�ł���f�B�X�N���v�^�̐�
	desc.Flags = d3d_descriptor_heap_flags;					// �f�B�X�N���v�^���w���Ă���f�[�^���V�F�[�_����Q�Ƃ��������ǂ���
	desc.NodeMask = 0;										// �A�_�v�^��1�����g��Ȃ��ꍇ��0���w��

	hr = d3d_device->CreateDescriptorHeap(
		&desc,												// �f�B�X�N���v�^�q�[�v�̐ݒ�
		IID_PPV_ARGS(m_d3d_descriptor_heap.GetAddressOf())	// �i�[����|�C���^�̃A�h���X
	);
	_ASSERT_EXPR_A(SUCCEEDED(hr), HRTrace(hr));

	m_d3d_descriptor_heap->SetName(name);

	// �f�B�X�N���v�^�����炩���ߍő吔�m�ۂ��Ă���
	m_descriptors.resize(max_descriptor_count);

	// �f�B�X�N���v�^������
	UINT d3d_descriptor_size = d3d_device->GetDescriptorHandleIncrementSize(d3d_descriptor_heap_type);
	D3D12_CPU_DESCRIPTOR_HANDLE d3d_cpu_handle = m_d3d_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE d3d_gpu_handle = m_d3d_descriptor_heap->GetGPUDescriptorHandleForHeapStart();
	for (UINT index = 0; index < max_descriptor_count; ++index)
	{
		Descriptor* descriptor = &m_descriptors.at(index);

		descriptor->Initialize(d3d_cpu_handle, d3d_gpu_handle, index);

		m_descriptor_stack.emplace_back(descriptor);

		d3d_cpu_handle.ptr += d3d_descriptor_size;
		d3d_gpu_handle.ptr += d3d_descriptor_size;
	}
	m_descriptor_stack_index = static_cast<int>(max_descriptor_count - 1);
}

// �f�X�g���N�^
DescriptorHeap::~DescriptorHeap()
{
	std::lock_guard<std::mutex> lock(m_mutex);
}

// �f�B�X�N���v�^���󂯎��
Descriptor* DescriptorHeap::PopDescriptor()
{
	if (m_descriptor_stack_index < 0)
	{
		return nullptr;
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	Descriptor* descriptor = m_descriptor_stack[m_descriptor_stack_index];
	m_descriptor_stack[m_descriptor_stack_index] = nullptr;
	m_descriptor_stack_index--;

	return descriptor;
}

// �f�B�X�N���v�^��ԋp����
void DescriptorHeap::PushDescriptor(Descriptor* descriptor)
{
	if (m_descriptor_stack_index >= m_descriptor_stack.size() - 1)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	m_descriptor_stack_index++;
	m_descriptor_stack[m_descriptor_stack_index] = descriptor;
}