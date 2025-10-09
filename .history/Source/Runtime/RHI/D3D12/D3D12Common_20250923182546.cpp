#include "D3D12Common.h"

using Microsoft::WRL::ComPtr;

namespace D3D12Utils
{
	void EnableDebugLayer()
	{
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			// 可选：启用GPU验证（性能影响较大）
			// ComPtr<ID3D12Debug1> debugController1;
			// if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
			// {
			//     debugController1->SetEnableGPUBasedValidation(TRUE);
			// }
		}
#endif
	}

	ComPtr<IDXGIAdapter1> GetHardwareAdapter(IDXGIFactory1* pFactory)
	{
		ComPtr<IDXGIAdapter1> adapter = nullptr;
		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// 跳过软件适配器
				continue;
			}

			// 检查是否支持D3D12
			#if False
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}
			#end
			for (auto level : SUPPORT_FEATURE_LEVELS) 
			{
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), level, _uuidof(ID3D12Device), nullptr))) {
					// 使用支持的最高级别
					break;
				}
			}

		return nullptr;
	}

	uint32_t GetDescriptorHandleIncrementSize(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type)
	{

	}
}