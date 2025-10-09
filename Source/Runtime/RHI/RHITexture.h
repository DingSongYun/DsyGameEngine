#pragma once

#include "RHI/RHIDefinitions.h"

/**
 * ����������
 */
struct RHITextureDesc
{
	uint32_t			width = 1;						// ������
	uint32_t			height = 1;						// ����߶�
	uint32_t			depth = 1;						// ������ȣ�����3D����
	uint32_t			mipLevels = 1;					// MIP��������
	uint32_t			arraySize = 1;					// ���������С��������������
	ERHIFormat			format = ERHIFormat::UNKNOWN;	// �����ʽ
	uint32_t			sampleCount = 1;				// ��������
	uint32_t			sampleQuality = 0;				// ���������ȼ�
	uint32_t			usageFlags = 0;					// ʹ�ñ�־��λ���룩
	uint32_t 			bindFlags = 0;					// �󶨱�־��λ���룩
	uint32_t			miscFlags = 0;					// ������־��λ���룩
	uint32_t			cpuAccessFlags = 0;				// CPU���ʱ�־��λ���룩
};

/**
 * ����ӿ�
 */
class IRHITexture
{
public:
	virtual ~IRHITexture() = default;

	virtual uint64_t GetWidth() const = 0;
	virtual uint64_t GetHeight() const = 0;
	virtual uint32_t GetDepth() const = 0;
	virtual uint32_t GetMipLevels() const = 0;
	virtual ERHIFormat GetFormat() const = 0;

	virtual RHIHandler GetNativeHandle() const = 0;
};