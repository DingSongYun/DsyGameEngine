#pragma once

i#include 

using RHIHandler = void*;
using RHIResult = _int32;

// ���ó���
constexpr uint32_t RHI_MAX_RENDER_TARGETS = 8;
constexpr uint32_t RHI_MAX_VIEWPORTS = 16;

// ��Դ��ʽ���ο����DX12�����е�DXGI_FORMAT��
enum class ERHIFormat : uint8_t
{
    Unknown,
    R8G8B8A8_UNORM,
    D24_UNORM_S8_UINT,
    // ... ������ʽ
};

// �����������
enum class ERHICommandQueueType : uint8_t
{
    Graphics,
    Compute,
    Copy
};
