#pragma once

#include "Core/CoreTypes.h"

using RHIHandler = void*;
using RHIResult = int32_t;

// ���ó���
constexpr uint32_t RHI_MAX_RENDER_TARGETS = 8;
constexpr uint32_t RHI_MAX_VIEWPORTS = 16;

// ��: DXGI_FORMAT
enum class ERHIFormat : uint8_t
{
	UNKNOWN,
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

enum class ERHITextureUsage : uint32_t
{
	None				= 0,
	RenderTarget		= 1 << 0,		// ����������ȾĿ��
	DepthStencil		= 1 << 1,		// �����������ģ��
	ShaderResource		= 1 << 2,		// ����������ɫ����Դ
	UnorderedAccess		= 1 << 3,		// �����������������Դ
	CopySource			= 1 << 4,		// ������������Դ
	CopyDestination		= 1 << 5,		// ������������Ŀ��
};
