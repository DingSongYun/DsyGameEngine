#pragma once

#include "Core/CoreTypes.h"
#include "RHI/RHIDefinitions.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

// 更完善的做法 - 从高到低尝试
D3D_FEATURE_LEVEL SUPPORT_FEATURE_LEVELS[] = {
D3D_FEATURE_LEVEL_12_1,
D3D_FEATURE_LEVEL_12_0,
D3D_FEATURE_LEVEL_11_1,
D3D_FEATURE_LEVEL_11_0
};

// 常用的DX12工具函数
namespace D3D12Utils
{
	// DXGI格式转换
	inline DXGI_FORMAT ConvertFormat(ERHIFormat Format)
	{
		switch (Format)
		{
		case ERHIFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ERHIFormat::D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case ERHIFormat::Unknown:
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline ERHIFormat ConvertFormat(DXGI_FORMAT Format)
	{
		switch (Format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return ERHIFormat::R8G8B8A8_UNORM;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return ERHIFormat::D24_UNORM_S8_UINT;
		case DXGI_FORMAT_UNKNOWN:
		default:
			return ERHIFormat::Unknown;
		}
	}

	// 调试层初始化
	void EnableDebugLayer();

	// 获取硬件适配器
	ComPtr<IDXGIAdapter1> GetHardwareAdapter(IDXGIFactory1 * pFactory);

	// 描述符堆大小获取
	uint32_t GetDescriptorHandleIncrementSize(ID3D12Device * device, D3D12_DESCRIPTOR_HEAP_TYPE type);
}