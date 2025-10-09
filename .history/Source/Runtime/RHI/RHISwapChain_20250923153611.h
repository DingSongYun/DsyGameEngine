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
	virtual IRHITexture* GetBackBuffer(uint32_t Index) const = 0;

	/** ִ��Present */
	virtual void Present(uint32_t SyncInterval=1) = 0;

	/** ������������С */
	virtual bool Resize(uint32_t NewWidth, uint32_t NewHeight) = 0;

	/** ��ȡ���������� */
	virtual uint32_t GetWidth() const = 0;

	/** ��ȡ�������߶� */
	virtual uint32_t GetHeight() const = 0;

	/** ��ȡ��������ʽ */
	virtual ERHIFormat GetFormat() const = 0;

	/** ��ȡԭ����������� */
	virtual RHIHandler GetNativeHandle() const = 0;
};