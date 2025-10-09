#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "Logging/Logger.h"

D3D12CommandList::~D3D12CommandList()
{
	Shutdown();
}

bool D3D12CommandList::Initialize(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
	if (!device)
	{
		LOG_ERROR("D3D12CommandList::Initialize() - Invalid D3D12Device provided.");
		return false;
	}

	m_device = device;
	m_type = type;

	// 创建命令分配器
	HRESULT hr = m_device->GetD3D12Device()->CreateCommandAllocator(
		m_type,
		IID_PPV_ARGS(&m_commandAllocator)
	);

	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Initialize() - Failed to create command allocator. HRESULT: {}", hr);
		return false;
	}

	// 创建命令列表
	hr = m_device->GetD3D12Device()->CreateCommandList(
		0,
		m_type,
		m_commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	);

	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Initialize() - Failed to create command list. HRESULT: {}", hr);
		return false;
	}

	// 命令列表创建后默认是打开状态，需要先关闭
	hr = m_commandList->Close();
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Initialize() - Failed to close command list. HRESULT: {}", hr);
		return false;
	}

	m_isRecording = false;
	LOG_INFO("D3D12CommandList initialized successfully.");
	return true;
}

void D3D12CommandList::Shutdown()
{
	if (m_isRecording)
	{
		Close();
	}

	m_commandList.Reset();
	m_commandAllocator.Reset();
	m_device = nullptr;
	m_isRecording = false;
}

bool D3D12CommandList::Reset()
{
	if (!m_commandAllocator || !m_commandList)
	{
		LOG_ERROR("D3D12CommandList::Reset() - Command list or allocator is null.");
		return false;
	}

	// 重置命令分配器
	HRESULT hr = m_commandAllocator->Reset();
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Reset() - Failed to reset command allocator. HRESULT: {}", hr);
		return false;
	}

	// 重置命令列表
	hr = m_commandList->Reset(m_commandAllocator.Get(), nullptr);
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Reset() - Failed to reset command list. HRESULT: {}", hr);
		return false;
	}

	m_isRecording = true;
	return true;
}

bool D3D12CommandList::Close()
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::Close() - Command list is null.");
		return false;
	}

	if (!m_isRecording)
	{
		LOG_WARN("D3D12CommandList::Close() - Command list is not recording.");
		return true;
	}

	HRESULT hr = m_commandList->Close();
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12CommandList::Close() - Failed to close command list. HRESULT: {}", hr);
		return false;
	}

	m_isRecording = false;
	return true;
}

void D3D12CommandList::ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor)
{
	if (!m_commandList || !renderTarget || !clearColor)
	{
		LOG_ERROR("D3D12CommandList::ClearRenderTarget() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHITexture到D3D12Texture的转换
	// 以及获取RTV描述符句柄的方法
	LOG_WARN("D3D12CommandList::ClearRenderTarget() - Not implemented yet.");
}

void D3D12CommandList::ClearDepthStencil(IRHITexture* depthStencil, float depth, uint8_t stencil)
{
	if (!m_commandList || !depthStencil)
	{
		LOG_ERROR("D3D12CommandList::ClearDepthStencil() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHITexture到D3D12Texture的转换
	// 以及获取DSV描述符句柄的方法
	LOG_WARN("D3D12CommandList::ClearDepthStencil() - Not implemented yet.");
}

void D3D12CommandList::SetRenderTargets(uint32_t numRenderTargets, IRHITexture** renderTargets, IRHITexture* depthStencil)
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::SetRenderTargets() - Command list is null.");
		return;
	}

	// TODO: 需要实现IRHITexture到D3D12Texture的转换
	// 以及获取RTV/DSV描述符句柄的方法
	LOG_WARN("D3D12CommandList::SetRenderTargets() - Not implemented yet.");
}

void D3D12CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::SetViewport() - Command list is null.");
		return;
	}

	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;

	m_commandList->RSSetViewports(1, &viewport);
}

void D3D12CommandList::SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom)
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::SetScissorRect() - Command list is null.");
		return;
	}

	D3D12_RECT scissorRect = {};
	scissorRect.left = left;
	scissorRect.top = top;
	scissorRect.right = right;
	scissorRect.bottom = bottom;

	m_commandList->RSSetScissorRects(1, &scissorRect);
}

void D3D12CommandList::SetPipelineState(IRHIPipelineState* pipelineState)
{
	if (!m_commandList || !pipelineState)
	{
		LOG_ERROR("D3D12CommandList::SetPipelineState() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHIPipelineState到ID3D12PipelineState的转换
	LOG_WARN("D3D12CommandList::SetPipelineState() - Not implemented yet.");
}

void D3D12CommandList::SetVertexBuffer(uint32_t slot, IRHIBuffer* vertexBuffer)
{
	if (!m_commandList || !vertexBuffer)
	{
		LOG_ERROR("D3D12CommandList::SetVertexBuffer() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHIBuffer到D3D12Buffer的转换
	// 以及获取顶点缓冲区视图的方法
	LOG_WARN("D3D12CommandList::SetVertexBuffer() - Not implemented yet.");
}

void D3D12CommandList::SetIndexBuffer(IRHIBuffer* indexBuffer)
{
	if (!m_commandList || !indexBuffer)
	{
		LOG_ERROR("D3D12CommandList::SetIndexBuffer() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHIBuffer到D3D12Buffer的转换
	// 以及获取索引缓冲区视图的方法
	LOG_WARN("D3D12CommandList::SetIndexBuffer() - Not implemented yet.");
}

void D3D12CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::DrawInstanced() - Command list is null.");
		return;
	}

	m_commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
}

void D3D12CommandList::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
{
	if (!m_commandList)
	{
		LOG_ERROR("D3D12CommandList::DrawIndexedInstanced() - Command list is null.");
		return;
	}

	m_commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
}

void D3D12CommandList::ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t stateAfter)
{
	if (!m_commandList || !resource)
	{
		LOG_ERROR("D3D12CommandList::ResourceBarrier() - Invalid parameters.");
		return;
	}

	// TODO: 需要实现IRHITexture到D3D12Texture的转换
	// 以及RHI资源状态到D3D12资源状态的转换
	LOG_WARN("D3D12CommandList::ResourceBarrier() - Not implemented yet.");
}
