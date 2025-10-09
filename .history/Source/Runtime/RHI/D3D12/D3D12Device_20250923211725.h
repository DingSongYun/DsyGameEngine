#pragma once

#include "RHI/RHIDevice.h"
#include "D3D12Common.h"

// Forward declarations
class D3D12CommandQueue;
class D3D12SwapChain;
class D3D12CommandList;
class D3D12Fence;


/**
 * D3D12�豸��װ
 * ����D3D12�豸�͸�����Դ
 */
class D3D12Device : public IRHIDevice
{
private:

	// ----------------------------------------
	// D3D12���Ķ���
	// DXGI: DirectX Graphics Infrastructure
	//	���������
	//		IDXGIFactory: DXGI�������󣬸��𴴽��͹���DXGI����
	//		IDXGIAdapter: DXGI���������󣬴���ϵͳ�е������Կ�
	//		IDXGISwapChain: DXGI���������󣬸����������GPU֮�仺�����Ľ���
	//		IDXGIOutput: DXGI������󣬴����ڻ���Ļ���

	// D3D12�豸
	ComPtr<ID3D12Device> m_Device;

	/**
	 * DXGIFatory����
	 * DXGIϵͳ����ڵ㣬���𴴽��͹������DXGI����
	 */
	ComPtr<IDXGIFactory6> m_Factory;

	/**
	 *  DXGIAdapter ��ʾ����������
	 * ����ϵͳ�е������Կ������������Կ���صĲ���
	 */
	ComPtr<IDXGIAdapter4> m_Adapter;

	// --------------------------------------------
	// �������

	std::unique_ptr<D3D12CommandQueue> m_GraphicsQueue;
	std::unique_ptr<D3D12CommandQueue> m_GraphicsQueue;
	std::unique_ptr<D3D12CommandQueue> m_GraphicsQueue;
};