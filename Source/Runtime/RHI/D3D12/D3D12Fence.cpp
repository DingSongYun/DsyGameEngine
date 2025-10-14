#include "D3D12Fence.h"
#include "D3D12Device.h"

D3D12Fence::~D3D12Fence()
{
	Shutdown();
}

bool D3D12Fence::Initialize(D3D12Device* device)
{
	if (!device)
	{
		LOG_ERROR("D3D12Fence::Initialize() - Invalid D3D12Device provided.");
		return false;
	}

	// 创建Fence对象
	HRESULT hr = device->GetD3D12Device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));

	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D12 Fence. HRESULT: 0x%08X", hr);
		return false;
	}

	// 创建事件句柄
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!m_FenceEvent)
	{
		LOG_ERROR("Failed to create fence event. Error: %d", GetLastError());
		return false;
	}

	return true;
}

void D3D12Fence::Shutdown()
{
	if (m_FenceEvent)
	{
		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;
	}
	m_Fence.Reset();
	m_Fence = nullptr;
}

void D3D12Fence::WaitForValue(uint64_t value)
{
	DE_ASSERT(m_Fence);

	if (m_Fence->GetCompletedValue() < value)
	{
		HRESULT hr = m_Fence->SetEventOnCompletion(value, m_FenceEvent);
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to set event on fence completion. HRESULT: 0x%08X", hr);
			return;
		}
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

uint64_t D3D12Fence::GetCompletedValue()
{
	DE_ASSERT(m_Fence);
	return m_Fence->GetCompletedValue();
}