#pragma once

#include "RHI/RHIDefinitions.h"

/**
 * 纹理描述符
 */
struct RHITextureDesc
{
	uint32_t			width = 1;						// 纹理宽度
	uint32_t			height = 1;						// 纹理高度
	uint32_t			depth = 1;						// 纹理深度（对于3D纹理）
	uint32_t			mipLevels = 1;					// MIP级别数量
	uint32_t			arraySize = 1;					// 纹理数组大小（对于数组纹理）
	ERHIFormat			format = ERHIFormat::UNKNOWN;	// 纹理格式
	uint32_t			sampleCount = 1;				// 采样数量
	uint32_t			sampleQuality = 0;				// 采样质量等级
	uint32_t			usageFlags = 0;					// 使用标志（位掩码）
	uint32_t 			bindFlags = 0;					// 绑定标志（位掩码）
	uint32_t			miscFlags = 0;					// 其他标志（位掩码）
	uint32_t			cpuAccessFlags = 0;				// CPU访问标志（位掩码）
};

/**
 * 纹理接口
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