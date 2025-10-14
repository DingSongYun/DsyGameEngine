#pragma once

#include "RHIDefinitions.h"


// Forward declarations
class IRHITexture;

/**
 * RHI交换链接口
 * 管理后台缓冲区
 */
class IRHISwapChain
{
public:
	virtual ~IRHISwapChain() = default;

	/** 获取缓冲区数量 */
	virtual uint32_t GetBufferCount() const = 0;

	/** 获取当前后台缓冲区索引 */
	virtual uint32_t GetCurrentBackBufferIndex() const = 0;

	/** 获取指定索引的后台缓冲区 */
	virtual IRHITexture* GetBackBuffer(uint32_t Index) const = 0;

	/** 显示当前缓冲区 */
	virtual void Present(uint32_t SyncInterval=1) = 0;

	/** 调整缓冲区大小 */
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