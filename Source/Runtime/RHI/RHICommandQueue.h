#pragma once

#include "RHIDefinitions.h"

// Forward declarations
class IRHICommandList;
class IRHIFence;


/**
 * RHI������нӿ�
 * �����ύ�����б�GPUִ��
 */
class IRHICommandQueue
{
public:
	virtual ~IRHICommandQueue() = default;

	/** ��ȡ����������� */
	virtual ERHICommandQueueType GetType() const = 0;

	/** ���������б� */
	virtual std::unique_ptr<IRHICommandList> CreateCommandList() = 0;

	/** �ύ�����б� */
	virtual void ExecuteCommandLists(IRHICommandList* const* CommandLists, uint32_t Count) = 0;

	/** �ź���������֪ͨGPUִ�е�ĳ��Fence */
	virtual void Signal(IRHIFence* Fence, uint64_t Value) = 0;

	/** �ȴ�������CPU�ȴ�GPUִ�е�ĳ��Fence */
	virtual void Wait(IRHIFence* Fence, uint64_t Value) = 0;

	/** ��ȡԭ��������о�� */
	virtual RHIHandler GetNativeHandle() const = 0;
};