#pragma once

i#include 

using RHIHandler = void*;
using RHIResult = _int32;

// 常用常量
constexpr uint32_t RHI_MAX_RENDER_TARGETS = 8;
constexpr uint32_t RHI_MAX_VIEWPORTS = 16;

// 资源格式（参考你的DX12代码中的DXGI_FORMAT）
enum class ERHIFormat : uint8_t
{
    Unknown,
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
