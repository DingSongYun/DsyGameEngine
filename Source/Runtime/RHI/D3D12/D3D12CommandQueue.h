#pragma once

#include "RHI/RHICommandQueue.h"
#include "D3D12Common.h"

// Forward declarations
class D3D12Device;
class D3D12CommandList;
class D3D12Fence;

/**
 * D3D12������з�װ
 */
class D3D12CommandQueue : public IRHICommandQueue
{
private:
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ERHICommandQueueType m_QueueType;
	D3D12Device* m_Device = nullptr;

	// ͬ������
	std::unique_ptr<D3D12Fence> m_Fence;
	uint64_t m_FenceValue = 0;

public:
	D3D12CommandQueue() = default;
	virtual ~D3D12CommandQueue();

	/** ��ʼ�� */
	bool Initialize(D3D12Device* device, ERHICommandQueueType type);

	/** �ر� */
	void Shutdown();

	// ----------------------------------------------
	// IRHICommandQueue �ӿ�
	virtual RHIHandler GetNativeHandle() const override { return m_CommandQueue.Get(); }
	virtual ERHICommandQueueType GetType() const override { return m_QueueType; }
	virtual std::unique_ptr<IRHICommandList> CreateCommandList() override;
	virtual void ExecuteCommandLists(IRHICommandList* const* commandLists, uint32_t count) override;
	virtual void Signal(IRHIFence* fence, uint64_t value) override;
	virtual void Wait(IRHIFence* fence, uint64_t value) override;

	// ----------------------------------------------
	// D3D12���нӿ�
	void WaitForIdle();
	ID3D12CommandQueue* GetD3D12CommandQueue() const { return m_CommandQueue.Get(); }

private:
	D3D12_COMMAND_LIST_TYPE GetD3D12CommandListType() const;
};