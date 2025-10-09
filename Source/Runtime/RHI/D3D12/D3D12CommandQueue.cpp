#include "D3D12CommandQueue.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12CommandList.h"

D3D12CommandQueue::~D3D12CommandQueue()
{
	Shutdown();
}

bool D3D12CommandQueue::Initialize(D3D12Device* device, ERHICommandQueueType type)
{
	if (!device)
	{
		LOG_ERROR("D3D12CommandQueue::Initialize() - Invalid D3D12Device provided.");
		return false;
	}

	m_Device = device;
	m_QueueType = type;

	// 创建命令队列描述符
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = GetD3D12CommandListType();
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	// 创建命令队列
	HRESULT hr = m_Device->GetD3D12Device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue));
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D12 Command Queue. HRESULT: 0x%08X", hr);
		return false;
	}

	// 创建同步Fence
	m_Fence = std::make_unique<D3D12Fence>();
	if (!m_Fence->Initialize(m_Device))
	{
		LOG_ERROR("Failed to create Fence for Command Queue.");
		return false;
	}

	LOG_INFO("D3D12CommandQueue initialized successfully.");
	return false;
}

void D3D12CommandQueue::Shutdown()
{
	if (m_CommandQueue)
	{
		WaitForIdle();
		m_Fence.reset();
		m_CommandQueue.Reset();
		m_Device = nullptr;
	}
	m_Device = nullptr;
}

std::unique_ptr<IRHICommandList> D3D12CommandQueue::CreateCommandList()
{
	auto commandList = std::make_unique<D3D12CommandList>();
	if (!commandList->Initialize(m_Device, GetD3D12CommandListType()))
	{
		LOG_ERROR("D3D12CommandQueue::CreateCommandList() - Failed to create command list.");
		return nullptr;
	}
	return std::move(commandList);
}

void D3D12CommandQueue::ExecuteCommandLists(IRHICommandList* const* commandLists, uint32_t count)
{
	if (commandLists == nullptr || count == 0)
	{
		LOG_WARN("D3D12CommandQueue::ExecuteCommandLists() - No command lists to execute.");
		return;
	}

	std::vector<ID3D12CommandList*> d3dCommandLists;
	d3dCommandLists.reserve(count);

	for (uint32_t i = 0; i < count; ++i)
	{
		if (auto* d3dCommandList = static_cast<D3D12CommandList*>(commandLists[i]))
		{
			d3dCommandLists.push_back(d3dCommandList->GetD3D12CommandList());
		}
	}

	if (!d3dCommandLists.empty())
	{
		m_CommandQueue->ExecuteCommandLists(static_cast<UINT>(d3dCommandLists.size()), d3dCommandLists.data());
	}
}

void D3D12CommandQueue::Signal(IRHIFence* fence, uint64_t value)
{
	if (auto* d3dFence = static_cast<D3D12Fence*>(fence))
	{
		m_CommandQueue->Signal(d3dFence->GetD3D12Fence(), value);
	}
}

void D3D12CommandQueue::Wait(IRHIFence* fence, uint64_t value)
{
	if (auto* d3dFence = static_cast<D3D12Fence*>(fence))
	{
		m_CommandQueue->Wait(d3dFence->GetD3D12Fence(), value);
	}
}

void D3D12CommandQueue::WaitForIdle()
{
	if (m_CommandQueue && m_Fence)
	{
		++ m_FenceValue;
		m_CommandQueue->Signal(m_Fence->GetD3D12Fence(), m_FenceValue);
		m_Fence->WaitForValue(m_FenceValue);
	}
}

D3D12_COMMAND_LIST_TYPE D3D12CommandQueue::GetD3D12CommandListType() const
{
	switch (m_QueueType)
	{
	case ERHICommandQueueType::Graphics:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case ERHICommandQueueType::Compute:
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case ERHICommandQueueType::Copy:
		return D3D12_COMMAND_LIST_TYPE_COPY;
	default:
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}
}

