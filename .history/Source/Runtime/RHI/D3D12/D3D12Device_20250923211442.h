#pragma once

#include "RHI/RHIDevice.h"
#include "D3D12Common.h"

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
	//		IDXGISwapChain: DXGI���������󣬸����������GPU֮��Ļ������Ľ���

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
};