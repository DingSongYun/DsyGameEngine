#pragma once

#include "Core/CoreTypes.h"
#include "RHIDefinitions.h"
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

// 常用的DX12工具函数
namespace D3D12Utils
{
	// DXGI格式转换
	DXGI_FORMAT ConvertFormat(ERHIFormat Format);
	ERHIFormat ConvertFormat(DXGI_FORMAT Format);

	// 调试层初始化
	void EnableDebugLayer();
}