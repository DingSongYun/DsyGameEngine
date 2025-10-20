#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12Fence.h"
#include "D3D12Buffer.h"
#include "D3D12Texture.h"
#include "D3D12SwapChain.h"

D3D12Device::~D3D12Device()
{
	Shutdown();
}

bool D3D12Device::Initialize()
{
	if (m_Initialized)
	{
		LOG_WARN("D3D12Device has already been initialized.");
		return true;
	}

	LOG_INFO("Initializing D3D12Device...");

	// 启用调试
	D3D12Utils::EnableDebugLayer();

	// 1. 创建DXGI工厂
	if (!CreateDXGIFactory())
	{
		LOG_ERROR("Failed to create DXGI Factory.");
		return false;
	}

	// 2. 初始化显卡适配器
	if (!InitHardwareAdapter())
	{
		LOG_ERROR("Failed to initialize hardware adapter.");
		return false;
	}

	// 3. 创建D3D12设备
	if (!CreateD3D12Device())
	{
		LOG_ERROR("Failed to create D3D12 Device.");
		return false;
	}

	// 4. 创建命令队列
	if (!CreateCommandQueues())
	{
		LOG_ERROR("Failed to create command queues.");
		return false;
	}

	m_Initialized = true;

	LOG_INFO("D3D12Device initialized successfully.");

	return true;
}

void D3D12Device::Shutdown()
{
	if (!m_Initialized)
		return;

	LOG_INFO("Shutting down D3D12Device...");

	// 确保所有GPU任务完成
	WaitForIdle();

	// 释放命令队列
	m_GraphicsQueue.reset();
	m_ComputeQueue.reset();
	m_CopyQueue.reset();

	// 清理D3D12对象
	m_Device.Reset();
	m_Adapter.Reset();
	m_Factory.Reset();

	m_Initialized = false;

	LOG_INFO("D3D12Device shut down successfully.");
}

bool D3D12Device::IsValid() const
{
	return m_Initialized && m_Device != nullptr;
}

IRHICommandQueue* D3D12Device::GetCommandQueue(ERHICommandQueueType Type)
{
	switch (Type)
	{
	case ERHICommandQueueType::Graphics:
		return m_GraphicsQueue.get();
	case ERHICommandQueueType::Compute:
		return m_ComputeQueue.get();
	case ERHICommandQueueType::Copy:
		return m_CopyQueue.get();
	}
	LOG_ERROR("GetCommandQueue(). Invalid command queue type requested.");
	return nullptr;
}

std::unique_ptr<IRHISwapChain> D3D12Device::CreateSwapChain(const RHISwapChainDesc& Desc)
{
	auto swapChain = std::make_unique<D3D12SwapChain>();
	if (!swapChain->Initialize(this, Desc))
	{
		LOG_ERROR("D3D12Device::CreateSwapChain: Failed to create swap chain.");
		return nullptr;
	}
	return swapChain;
}

std::unique_ptr<IRHIBuffer> D3D12Device::CreateBuffer(const RHIBufferDesc& Desc)
{
	auto buffer = std::make_unique<D3D12Buffer>();
	if (!buffer->Initialize(this, Desc))
	{
		LOG_ERROR("D3D12Device::CreateBuffer: Failed to create buffer.");
		return nullptr;
	}
	return buffer;
}

std::unique_ptr<IRHITexture> D3D12Device::CreateTexture(const RHITextureDesc& Desc)
{
	auto texture = std::make_unique<D3D12Texture>();
	if (!texture->Initialize(this, Desc))
	{
		LOG_ERROR("D3D12Device::CreateTexture: Failed to create texture.");
		return nullptr;
	}

	return texture;
}

std::unique_ptr<IRHIFence> D3D12Device::CreateFence()
{
	auto fence = std::make_unique<D3D12Fence>();
	if (!fence->Initialize(this))
	{
		LOG_ERROR("D3D12Device::CreateFence: Failed to create fence.");
		return nullptr;
	}
	return std::move(fence);
}

void D3D12Device::WaitForIdle()
{
	if (m_GraphicsQueue) m_GraphicsQueue->WaitForIdle();
	if (m_ComputeQueue) m_ComputeQueue->WaitForIdle();
	if (m_CopyQueue) m_CopyQueue->WaitForIdle();
}

bool D3D12Device::CreateDXGIFactory()
{
	UINT createFactoryFlags = 0;
#ifdef _DEBUG
	createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&m_Factory));
	return SUCCEEDED(hr);
}

bool D3D12Device::CreateD3D12Device()
{
	HRESULT hr = D3D12CreateDevice(
		m_Adapter.Get(),
		D3D_FEATURE_LEVEL_11_0, // 最低支持的特性级别
		IID_PPV_ARGS(&m_Device)
	);

	if (FAILED(hr))
	{
		LOG_WARN("D3D12Device::CreateD3D12Device: Failed to create D3D12 device. HRESULT: 0x%X", hr);
		return false;
	}

#ifdef DE_DEBUG
	// 启用D3D12调试信息
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(m_Device.As(&infoQueue)))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	}
#endif
	return true;
}

bool D3D12Device::CreateCommandQueues()
{
	// 创建Graphics命令队列
	m_GraphicsQueue = std::make_unique<D3D12CommandQueue>();
	if (!m_GraphicsQueue->Initialize(this, ERHICommandQueueType::Graphics))
	{
		return false;
	}

	// 创建Compute命令队列
	m_ComputeQueue = std::make_unique<D3D12CommandQueue>();
	if (!m_ComputeQueue->Initialize(this, ERHICommandQueueType::Compute))
	{
		return false;
	}

	// 创建Copy命令队列
	m_CopyQueue = std::make_unique<D3D12CommandQueue>();
	if (!m_CopyQueue->Initialize(this, ERHICommandQueueType::Copy))
	{
		return false;
	}

	return true;
}

bool D3D12Device::InitHardwareAdapter()
{
	ComPtr<IDXGIFactory1> factory;
	if ( SUCCEEDED(m_Factory.As(&factory)) )
	{
		ComPtr<IDXGIAdapter1> adapter = D3D12Utils::GetHardwareAdapter(factory.Get());
		if (SUCCEEDED(adapter.As(&m_Adapter)))
		{
			return true;
		}
	}

	LOG_WARN("D3D12Device::InitHardwareAdapter: No suitable hardware adapter found.");
	return false;
}
