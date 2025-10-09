#pragma once

#include "RHIDefinitions.h"


// Forward declarations
class IRHITexture;

/**
 * 交换链接口
 * 负责管理前后缓冲区的交换
 */
class IRHISwapChain
{
public:
	~IRHISwapChain() = default;

	/** 获取缓冲区数量 */
	virtual uint32_t GetBufferCount() const = 0;

	/** 获取后台缓冲区索引 */
	virtual uint32_t GetCurrentBackBufferIndex() const = 0;

	/** 获取特定索引的后台缓冲区 */
	virtual IRHITexture* GetBackBuffer(uint32_t Index) const = 0;

	/** 执行Present */
	virtual void Present(uint32_t SyncInterval=1) = 0;

	/** 调整交换链大小 */
	virtual bool Resize(uint32_t NewWidth, uint32_t NewHeight) = 0;

	/** 获取交换链宽度 */
	virtual uint32_t GetWidth() const = 0;

	/** 获取交换链高度 */
	virtual uint32_t GetHeight() const = 0;

	/** 获取交换链格式 */
	virtual ERHIFormat GetFormat() const = 0;

	/** 获取原生交换链句柄 */
	virtual RHIHandler GetNativeHandle() const = 0;
};