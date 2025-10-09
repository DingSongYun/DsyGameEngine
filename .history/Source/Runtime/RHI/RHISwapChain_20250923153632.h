#pragma once

#include "RHIDefinitions.h"


// Forward declarations
class IRHITexture;

/**
 * �������ӿ�
 * �������ǰ�󻺳����Ľ���
 */
class IRHISwapChain
{
public:
	~IRHISwapChain() = default;

	/** ��ȡ���������� */
	virtual uint32_t GetBufferCount() const = 0;

	/** ��ȡ��̨���������� */
	virtual uint32_t GetCurrentBackBufferIndex() const = 0;

	/** ��ȡ�ض������ĺ�̨������ */
	virtual IRHITexture* GetCurrentBackBuffer() const = 0;
};