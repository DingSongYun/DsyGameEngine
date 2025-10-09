#pragma once

#include "Core/CoreTypes.h"

using RHIHandler = void*;
using RHIResult = int32_t;

// 常用常量
constexpr uint32_t RHI_MAX_RENDER_TARGETS = 8;
constexpr uint32_t RHI_MAX_VIEWPORTS = 16;

// 见: DXGI_FORMAT
enum class ERHIFormat : uint8_t
{
	UNKNOWN,
	R8G8B8A8_UNORM,
	D24_UNORM_S8_UINT,
	// ... 其他格式
};

// 命令队列类型
enum class ERHICommandQueueType : uint8_t
{
	Graphics,
	Compute,
	Copy
};

enum class ERHITextureUsage : uint32_t
{
	None				= 0,
	RenderTarget		= 1 << 0,		// 纹理用作渲染目标
	DepthStencil		= 1 << 1,		// 纹理用作深度模板
	ShaderResource		= 1 << 2,		// 纹理用作着色器资源
	UnorderedAccess		= 1 << 3,		// 纹理用作无序访问资源
	CopySource			= 1 << 4,		// 纹理用作复制源
	CopyDestination		= 1 << 5,		// 纹理用作复制目标
};
