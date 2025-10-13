#include "D3D12SwapChain.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12Texture.h"

D3D12SwapChain::~D3D12SwapChain()
{
	Shutdown();
}

bool D3D12SwapChain::Initialize(D3D12Device* device, const RHISwapChainDesc& desc)
{
	if (!device)
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Invalid D3D12Device provided.");
		return false;
	}

	if (!desc.WindowHandle)
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Invalid window handle provided.");
		return false;
	}

	m_Device = device;
	m_Width = desc.Width;
	m_Height = desc.Height;
	m_BufferCount = desc.BufferCount;
	m_Format = desc.Format;

	// 创建交换链
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
		.Width = m_Width,
		.Height = m_Height,
		.Format = static_cast<DXGI_FORMAT>(m_Format),
		.Stereo = FALSE,
		.SampleDesc = {
			.Count = 1,
			.Quality = 0
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = m_BufferCount,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = 0
	};

	auto* graphicQueue = static_cast<D3D12CommandQueue*>(device->GetCommandQueue(ERHICommandQueueType::Graphics));
	if (!graphicQueue)
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Failed to get graphics command queue.");
		return false;
	}

	ComPtr<IDXGISwapChain1> swapChain1;
	HRESULT hr = device->GetDXGIFactory()->CreateSwapChainForHwnd(
		graphicQueue->GetD3D12CommandQueue(),
		reinterpret_cast<HWND>(desc.WindowHandle),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Failed to create swap chain. HRESULT: 0x%X", hr);
		return false;
	}

	hr = swapChain1.As(&m_SwapChain);
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Failed to query IDXGISwapChain3 interface. HRESULT: 0x%X", hr);
		return false;
	}

	// 禁用Alt+Enter全屏切换
	device->GetDXGIFactory()->MakeWindowAssociation(reinterpret_cast<HWND>(desc.WindowHandle), DXGI_MWA_NO_ALT_ENTER);

	// 创建后台缓冲区
	if (!CreateBackBuffers())
	{
		LOG_ERROR("D3D12SwapChain::Initialize() - Failed to create back buffers.");
		return false;
	}

	LOG_INFO("D3D12SwapChain initialized successfully.");
	return true;
}

void D3D12SwapChain::Shutdown()
{
	ReleaseBackBuffers();
	m_SwapChain.Reset();
	m_Device = nullptr;
}

uint32_t D3D12SwapChain::GetCurrentBackBufferIndex() const
{
	return m_SwapChain ? m_SwapChain->GetCurrentBackBufferIndex() : 0;
}

IRHITexture* D3D12SwapChain::GetBackBuffer(uint32_t Index) const
{
	if (Index >= m_BackBuffers.size())
	{
		LOG_ERROR("D3D12SwapChain::GetBackBuffer() - Index out of range.");
		return nullptr;
	}
	// 假设已实现 D3D12Texture 类，并且其构造函数接受 ID3D12Resource* 和格式等参数
	// 这里需要将 m_BackBuffers[Index] 包装为 D3D12Texture 实例并返回
	// 例如:
	D3D12Texture* texture = new D3D12Texture();
	if (!texture->InitializeFromReource(m_Device, m_BackBuffers[Index].Get()))
	{
		LOG_ERROR("D3D12SwapChain::GetBackBuffer() - Failed to initialize D3D12Texture from back buffer resource.");
		return nullptr;
	}
	
	return texture;
}

void D3D12SwapChain::Present(uint32_t SyncInterval)
{
	DE_ASSERT(m_SwapChain);

	HRESULT hr = m_SwapChain->Present(SyncInterval, 0);
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12SwapChain::Present() - Failed to present swap chain. HRESULT: 0x%X", hr);
	}
}

bool D3D12SwapChain::Resize(uint32_t Width, uint32_t Height)
{
	DE_ASSERT(m_SwapChain);
	if (m_Width == Width && m_Height == Height)
		return true; // 尺寸未变化

	// 释放现有后台缓冲区
	ReleaseBackBuffers();

	HRESULT hr = m_SwapChain->ResizeBuffers(
		m_BufferCount,
		Width,
		Height,
		static_cast<DXGI_FORMAT>(m_Format),
		0);

	if (FAILED(hr))
	{
		LOG_ERROR("D3D12SwapChain::Resize() - Failed to resize swap chain buffers. HRESULT: 0x%X", hr);
		return false;
	}

	m_Width = Width;
	m_Height = Height;

	// 重新创建后台缓冲区
	return CreateBackBuffers();
}

bool D3D12SwapChain::CreateBackBuffers()
{
	DE_ASSERT(m_SwapChain);

	m_BackBuffers.resize(m_BufferCount);
	for (uint32_t i = 0; i < m_BufferCount; ++i)
	{
		HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBuffers[i]));
		if (FAILED(hr))
		{
			LOG_ERROR("D3D12SwapChain::CreateBackBuffers() - Failed to get back buffer %d. HRESULT: 0x%X", i, hr);
			return false;
		}
	}

	return true;
}

void D3D12SwapChain::ReleaseBackBuffers()
{
	for (auto& buffer : m_BackBuffers)
	{
		DE_ASSERT(buffer);
		buffer.Reset();
	}
	m_BackBuffers.clear();
}
