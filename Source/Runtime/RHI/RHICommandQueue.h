#pragma once

#include "RHIDefinitions.h"

// Forward declarations
class IRHICommandList;
class IRHIFence;


/**
 * RHI命令队列接口
 * 负责提交命令列表到GPU执行
 */
class IRHICommandQueue
{
public:
	virtual ~IRHICommandQueue() = default;

	/** 获取命令队列类型 */
	virtual ERHICommandQueueType GetType() const = 0;

	/** 创建命令列表 */
	virtual std::unique_ptr<IRHICommandList> CreateCommandList() = 0;

	/** 提交命令列表 */
	virtual void ExecuteCommandLists(IRHICommandList* const* CommandLists, uint32_t Count) = 0;

	/** 信号量操作，通知GPU执行到某个Fence */
	virtual void Signal(IRHIFence* Fence, uint64_t Value) = 0;

	/** 等待操作，CPU等待GPU执行到某个Fence */
	virtual void Wait(IRHIFence* Fence, uint64_t Value) = 0;

	/** 获取原生命令队列句柄 */
	virtual RHIHandler GetNativeHandle() const = 0;
};