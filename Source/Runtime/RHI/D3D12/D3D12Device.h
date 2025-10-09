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

	/** D3D12�豸 */
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

	/** 
	 * ��Ⱦ���� 
	 * 	-> Vertex&Pixel��ɫ����ͼ�ι���
	 * 	-> ��դ��
	 * 	-> ��Ȳ���
	 * 	-> ���л�������
	 */
	std::unique_ptr<D3D12CommandQueue> m_GraphicsQueue;

	/** 
	 * �������, ComputeShader��������
	 * 	-> ����Ч��(e.g. ģ��������)
	 * 	-> ����ģ��
	 * 	-> ����ϵͳ����
	 */
	std::unique_ptr<D3D12CommandQueue> m_ComputeQueue;

	/** 
	 * ���ƶ��� 
	 * 	-> �����ϴ�GPU
	 *  -> ��Դ���ƺ��ƶ�
	 *  -> ���������ݴ���
	 *  -> �ڴ�ӳ��
	 */
	std::unique_ptr<D3D12CommandQueue> m_CopyQueue;

	/** �豸״̬ */
	bool m_Initialized = false;

public:
	D3D12Device() = default;
	virtual ~D3D12Device();

	// --------------------------------------------
	// IRHIDevice�ӿ�ʵ��
	virtual bool Initialize() override;
	virtual void Shutdown() override;
	virtual bool IsValid() const override;

	virtual RHIHandler GetNativeDevice() const override { return m_Device.Get(); }
	virtual IRHICommandQueue* GetCommandQueue(ERHICommandQueueType Type) override;
	virtual std::unique_ptr<IRHISwapChain> CreateSwapChain(const RHISwapChainDesc& Desc) override;
	virtual std::unique_ptr<IRHIBuffer> CreateBuffer(const RHIBufferDesc& Desc) override;
	virtual std::unique_ptr<IRHITexture> CreateTexture(const RHITextureDesc& Desc) override;
	virtual std::unique_ptr<IRHIFence> CreateFence() override;
	virtual void WaitForIdle();

	// --------------------------------------------
	// D3D12�ӿ�
	ID3D12Device* GetD3D12Device() const { return m_Device.Get(); }
	IDXGIFactory6* GetDXGIFactory() const { return m_Factory.Get(); }
	IDXGIAdapter4* GetDXGIAdapter() const { return m_Adapter.Get(); }

private:
	// --------------------------------------------
	// �ڲ���ʼ������

	bool CreateDXGIFactory();
	bool CreateD3D12Device();
	bool CreateCommandQueues();
	bool InitHardwareAdapter();
};