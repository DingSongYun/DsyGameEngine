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
	// DXGI: DirectX Graphics Infrastructure
	//	核心组件：
	//		IDXGIFactory: DXGI工厂对象，负责创建和管理DXGI对象
	//		IDXGIAdapter: DXGI适配器对象，代表系统中的物理显卡
	//		IDXGISwapChain: DXGI交换链对象，负责管理窗口与GPU之间的缓冲区的交换

	// D3D12设备
	ComPtr<ID3D12Device> m_Device;

	/**
	 * DXGIFatory对象
	 * DXGI系统的入口点，负责创建和管理各种DXGI对象
	 */
	ComPtr<IDXGIFactory6> m_Factory;

	/**
	 *  DXGIAdapter 显示适配器对象
	 * 代表系统中的物理显卡，负责处理与显卡相关的操作
	 */
	ComPtr<IDXGIAdapter4> m_Adapter;
};