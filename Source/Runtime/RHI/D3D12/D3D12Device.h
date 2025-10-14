#pragma once

#include "RHI/RHIDevice.h"
#include "D3D12Common.h"

// Forward declarations
class D3D12CommandQueue;
class D3D12SwapChain;
class D3D12CommandList;
class D3D12Fence;


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
	//		IDXGISwapChain: DXGI交换链对象，负责管理窗口与GPU之间缓冲区的交换
	//		IDXGIOutput: DXGI输出对象，代表窗口或屏幕输出

	/** D3D12设备 */
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

	// --------------------------------------------
	// 命令队列

	/** 
	 * 渲染队列 
	 * 	-> Vertex&Pixel着色器等图形管线
	 * 	-> 光栅化
	 * 	-> 深度测试
	 * 	-> 所有绘制任务
	 */
	std::unique_ptr<D3D12CommandQueue> m_GraphicsQueue;

	/** 
	 * 计算队列, ComputeShader计算任务
	 * 	-> 后处理效果(e.g. 模糊、降噪)
	 * 	-> 物理模拟
	 * 	-> 粒子系统更新
	 */
	std::unique_ptr<D3D12CommandQueue> m_ComputeQueue;

	/** 
	 * 复制队列 
	 * 	-> 纹理上传GPU
	 *  -> 资源复制和移动
	 *  -> 缓冲区数据传输
	 *  -> 内存映射
	 */
	std::unique_ptr<D3D12CommandQueue> m_CopyQueue;

	/** 设备状态 */
	bool m_Initialized = false;

public:
	D3D12Device() = default;
	virtual ~D3D12Device();

	// --------------------------------------------
	// IRHIDevice接口实现
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
	// D3D12接口
	ID3D12Device* GetD3D12Device() const { return m_Device.Get(); }
	IDXGIFactory6* GetDXGIFactory() const { return m_Factory.Get(); }
	IDXGIAdapter4* GetDXGIAdapter() const { return m_Adapter.Get(); }

private:
	// --------------------------------------------
	// 内部初始化方法

	bool CreateDXGIFactory();
	bool CreateD3D12Device();
	bool CreateCommandQueues();
	bool InitHardwareAdapter();
};