#pragma once

#include "RHIDefinitions.h"

// Forward declarations
class IRHICommandQueue;
class IRHISwapChain;
class IRHIBuffer;
class IRHITexture;
class IRHIFence;
struct RHITextureDesc;
struct RHIBufferDesc;

/**
 * SwapChian描述符
 */
struct RHISwapChainDesc
{
	uint32_t Width = 1920;
	uint16_t Height = 1080;
	uint32_t BufferCount = 2;
	ERHIFormat Format = ERHIFormat::R8G8B8A8_UNORM;
	RHIHandler WindowHandle = nullptr;
	bool bFullscreen = false;
};

/**
 * RHI设备接口
 * 负责管理底层图形API的设备和各种资源
 */
class IRHIDevice
{
public:
	virtual ~IRHIDevice() = default;

	/** 初始化设备 */
	virtual bool Initialize() = 0;

	/** 关闭设备 */
	virtual void Shutdown() = 0;

	/** 设备是否有效 */
	virtual bool IsValid() const = 0;

	/** 获取底层图形API的原生设备句柄 */
	virtual RHIHandler GetNativeDevice() const = 0;

	/** 获取指定类型的命令队列 */
	virtual IRHICommandQueue* GetCommandQueue(ERHICommandQueueType Type) = 0;

	/** 创建交换链 */
	virtual std::unique_ptr<IRHISwapChain> CreateSwapChain(const RHISwapChainDesc& Desc) = 0;

	/** 创建缓冲区 */
	virtual std::unique_ptr<IRHIBuffer> CreateBuffer(const RHIBufferDesc& Desc) = 0;

	/** 创建纹理资源 */
	virtual std::unique_ptr<IRHITexture> CreateTexture(const RHITextureDesc& Desc) = 0;

	/** 创建同步栅栏 */
	virtual std::unique_ptr<IRHIFence> CreateFence() = 0;

	/** 等待设备空闲 */
	virtual void WaitForIdle() = 0;
};