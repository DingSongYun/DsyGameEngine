#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12Buffer.h"

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

	m_type = type;

	ID3D12Device* d3dDevice = device->GetD3D12Device();

	
	HRESULT hr = d3dDevice->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_commandAllocator));
	
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D12 Command Allocator. HRESULT: 0x%08X", hr);
		return false;
	}

	// ���������б�
	uint32_t nodeMask = 0; // TODO: ??��;
	hr = d3dDevice->CreateCommandList(nodeMask, m_type, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

	if (FAILED(hr))
	{
		LOG_ERROR("Failed to create D3D12 Command List. HRESULT: 0x%08X", hr);
		return false;
	}

	hr = m_commandList->Close();
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to close D3D12 Command List after creation. HRESULT: 0x%08X", hr);
		return false;
	}

	m_isRecording = false;

	LOG_INFO("D3D12CommandList initialized successfully.");

	return true;
}

void D3D12CommandList::Shutdown()
{
	Close();

	if (m_commandAllocator)
	{
		m_commandAllocator.Reset();
	}
	m_isRecording = false;
}

void D3D12CommandList::Close()
{
	if (m_isRecording && m_commandList)
	{
		HRESULT hr = m_commandList->Close();
		if (FAILED(hr))
		{
			LOG_ERROR("Failed to close D3D12 Command List. HRESULT: 0x%08X", hr);
		}
		m_isRecording = false;
	}
}

void D3D12CommandList::ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor)
{
	DE_ASSERT(m_commandList);
	DE_ASSERT(renderTarget);

	D3D12Texture* d3dTexture = static_cast<D3D12Texture*>(renderTarget);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dTexture->GetRTVHandle();
	if (rtvHandle.ptr == 0)
	{
		LOG_ERROR("D3D12CommandList::ClearRenderTarget() - invalid RTV handle.");
		return;
	}

	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void D3D12CommandList::ClearDepthStencil(IRHITexture* renderTarget, float depth, uint8_t stencil)
{
	DE_ASSERT(m_commandList);
	DE_ASSERT(renderTarget);

	D3D12Texture* d3dTexture = static_cast<D3D12Texture*>(renderTarget);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3dTexture->GetDSVHandle();
	if (dsvHandle.ptr == 0)
	{
		LOG_ERROR("D3D12CommandList::ClearDepthStencil() - invalid DSV handle.");
		return;
	}

	m_commandList->ClearDepthStencilView(
		dsvHandle, 
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
		depth, stencil, 
		0, 
		nullptr);
}

void D3D12CommandList::SetRenderTargets(uint32_t numRenderTargets, IRHITexture* const* renderTargets, IRHITexture* depthStencil)
{
	DE_ASSERT(m_commandList);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[RHI_MAX_RENDER_TARGETS] = {};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};
	
	for (uint32_t i = 0; i < numRenderTargets && i < RHI_MAX_RENDER_TARGETS; ++i)
	{
		D3D12Texture* d3dTexture = static_cast<D3D12Texture*>(renderTargets[i]);
		rtvHandles[i] = d3dTexture->GetRTVHandle();
	}

	if (depthStencil)
	{
		D3D12Texture* d3dTexture = static_cast<D3D12Texture*>(depthStencil);
		dsvHandle = d3dTexture->GetDSVHandle();
	}

	m_commandList->OMSetRenderTargets(
		numRenderTargets, 
		rtvHandles, 
		FALSE, 
		&dsvHandle);
}

void D3D12CommandList::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
{
	DE_ASSERT(m_commandList);

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
	DE_ASSERT(m_commandList);

	D3D12_RECT scissorRect = {};
	scissorRect.left = left;
	scissorRect.top = top;
	scissorRect.right = right;
	scissorRect.bottom = bottom;

	m_commandList->RSSetScissorRects(1, &scissorRect);
}

void D3D12CommandList::SetPipelineState(RHIHandler pipelineState)
{
	DE_ASSERT(m_commandList);

	// TODO: ʵ�ֹ���״̬����
	// ��Ҫ��ʵ��D3D12PipelineState��
	LOG_WARN("D3D12CommandList::SetPipelineState() - Not implemented yet.");
}

void D3D12CommandList::SetVertexBuffer(uint32_t slot, IRHIBuffer* buffer)
{
	DE_ASSERT(m_commandList);
	DE_ASSERT(buffer);

	D3D12Buffer* d3dBuffer = static_cast<D3D12Buffer*>(buffer);
	if (!d3dBuffer || !d3dBuffer->IsValid())
	{
		LOG_ERROR("D3D12CommandList::SetVertexBuffer() - Invalid buffer.");
		return;
	}

	// Ensure resource state is correct for vertex buffer usage
	ID3D12Resource* resource = d3dBuffer->GetD3D12Resource();
	D3D12_RESOURCE_STATES currentState = d3dBuffer->GetCurrentState();
	D3D12_RESOURCE_STATES desiredState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	if (resource && currentState != desiredState)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = currentState;
		barrier.Transition.StateAfter = desiredState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->ResourceBarrier(1, &barrier);
		// Update tracked state
		d3dBuffer->SetCurrentState(desiredState);
	}

	const D3D12_VERTEX_BUFFER_VIEW& vbView = d3dBuffer->GetVertexBufferView();
	if (vbView.BufferLocation == 0 || vbView.SizeInBytes == 0)
	{
		LOG_ERROR("D3D12CommandList::SetVertexBuffer() - Invalid vertex buffer view.");
		return;
	}

	m_commandList->IASetVertexBuffers(slot, 1, &vbView);
}

void D3D12CommandList::SetIndexBuffer(IRHIBuffer* buffer)
{
	DE_ASSERT(m_commandList);
	DE_ASSERT(buffer);

	D3D12Buffer* d3dBuffer = static_cast<D3D12Buffer*>(buffer);
	if (!d3dBuffer || !d3dBuffer->IsValid())
	{
		LOG_ERROR("D3D12CommandList::SetIndexBuffer() - Invalid buffer.");
		return;
	}

	// Ensure resource state is correct for index buffer usage
	ID3D12Resource* resource = d3dBuffer->GetD3D12Resource();
	D3D12_RESOURCE_STATES currentState = d3dBuffer->GetCurrentState();
	D3D12_RESOURCE_STATES desiredState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	if (resource && currentState != desiredState)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = currentState;
		barrier.Transition.StateAfter = desiredState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_commandList->ResourceBarrier(1, &barrier);
		// Update tracked state
		d3dBuffer->SetCurrentState(desiredState);
	}

	const D3D12_INDEX_BUFFER_VIEW& ibView = d3dBuffer->GetIndexBufferView();
	if (ibView.BufferLocation == 0 || ibView.SizeInBytes == 0)
	{
		LOG_ERROR("D3D12CommandList::SetIndexBuffer() - Invalid index buffer view.");
		return;
	}

	m_commandList->IASetIndexBuffer(&ibView);
}

void D3D12CommandList::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
{
	DE_ASSERT(m_commandList);
	m_commandList->DrawInstanced(vertexCount, instanceCount, startVertex, startInstance);
}

void D3D12CommandList::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
{
	DE_ASSERT(m_commandList);
	m_commandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex, startInstance);
}

void D3D12CommandList::ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t stateAfter)
{
	DE_ASSERT(m_commandList);
	DE_ASSERT(resource);

	D3D12Texture* d3dTexture = static_cast<D3D12Texture*>(resource);
	ID3D12Resource* d3dResource = d3dTexture->GetD3D12Resource();

	if (!d3dResource)
	{
		LOG_ERROR("D3D12CommandList::ResourceBarrier() - Invalid D3D12 resource.");
		return;
	}

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = d3dResource;
	barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(stateBefore);
	barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(stateAfter);
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_commandList->ResourceBarrier(1, &barrier);

	d3dTexture->SetCurrentState(static_cast<D3D12_RESOURCE_STATES>(stateAfter));
}
