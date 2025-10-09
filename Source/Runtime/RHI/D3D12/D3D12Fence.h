#pragma once

#include "RHI/RHIDefinitions.h"
#include "D3D12Common.h"
#include "RHI/RHIFence.h"

class D3D12Device;

class D3D12Fence : public IRHIFence
{
private:
	ComPtr<ID3D12Fence> m_Fence;
	HANDLE m_FenceEvent = nullptr;

public:
	D3D12Fence() = default;
	virtual ~D3D12Fence();

	bool Initialize(D3D12Device* device);
	void Shutdown();

	void WaitForValue(uint64_t value);
	uint64_t GetCompletedValue();

	ID3D12Fence* GetD3D12Fence() const { return m_Fence.Get(); }
};