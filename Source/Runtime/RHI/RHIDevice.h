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
 * SwapChian������
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
 * RHI�豸�ӿ�
 * �������ײ�ͼ��API���豸�͸�����Դ
 */
class IRHIDevice
{
public:
	virtual ~IRHIDevice() = default;

	/** ��ʼ���豸 */
	virtual bool Initialize() = 0;

	/** �ر��豸 */
	virtual void Shutdown() = 0;

	/** �豸�Ƿ���Ч */
	virtual bool IsValid() const = 0;

	/** ��ȡ�ײ�ͼ��API��ԭ���豸��� */
	virtual RHIHandler GetNativeDevice() const = 0;

	/** ��ȡָ�����͵�������� */
	virtual IRHICommandQueue* GetCommandQueue(ERHICommandQueueType Type) = 0;

	/** ���������� */
	virtual std::unique_ptr<IRHISwapChain> CreateSwapChain(const RHISwapChainDesc& Desc) = 0;

	/** ���������� */
	virtual std::unique_ptr<IRHIBuffer> CreateBuffer(const RHIBufferDesc& Desc) = 0;

	/** ����������Դ */
	virtual std::unique_ptr<IRHITexture> CreateTexture(const RHITextureDesc& Desc) = 0;

	/** ����ͬ��դ�� */
	virtual std::unique_ptr<IRHIFence> CreateFence() = 0;

	/** �ȴ��豸���� */
	virtual void WaitForIdle() = 0;
};