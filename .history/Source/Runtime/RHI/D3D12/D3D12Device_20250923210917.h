#pragma once

#include "RHI/RHIDevice.h"
#include "D3D12Common.h"

/**
 * D3D12设备封装
 * 管理D3D12设备和各种资源
 */
class D3D12Device : public IRHIDevice
{
private:

	// ----------------------------------------
	// D3D12核心对象

	// D3D12设备
	ComPtr<ID3D12Device> m_Device;
	/**
	 * DXGIFatory对象
	 * DXGI系统的入口点，负责创建和管理各种DXGI对象
	 */
	ComPtr<IDXGIFactory6> m_Factory;
	ComPtr<IDXGIAdapter4> m_Adapter;
};