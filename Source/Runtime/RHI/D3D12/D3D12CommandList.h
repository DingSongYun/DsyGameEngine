#pragma once

#include "RHI/RHICommandList.h"
#include "D3D12Common.h"

// Forward declarations
class D3D12Device;

class D3D12CommandList : public IRHICommandList
{
private:
	/** D3D12 命令列表 */
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	/** D3D12 命令分配器 */
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;

	/** 命令列表类型 */
	D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	bool m_isRecording = false;

public:
	D3D12CommandList() = default;
	virtual ~D3D12CommandList();

	bool Initialize(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
	void Shutdown();
	void Close();

	// ----------------------------------------------
	// IRHICommandList 接口
	virtual void ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor) override;
	virtual void ClearDepthStencil(IRHITexture* renderTarget, float depth, uint8_t stencil) override;
	virtual void SetRenderTargets(uint32_t numRenderTargets, IRHITexture* const* renderTargets, IRHITexture* depthStencil) override;
	virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
	virtual void SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom) override;
	virtual void SetPipelineState(RHIHandler pipelineState) override;
	virtual void SetVertexBuffer(uint32_t slot, IRHIBuffer* buffer) override;
	virtual void SetIndexBuffer(IRHIBuffer* buffer) override;
	virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) override;
	virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) override;
	virtual void ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t starteAfter) override;
	virtual RHIHandler GetNativeHandle() const override { return m_commandList.Get(); }


	// ----------------------------------------------
	// D3D12特有接口

	/** 获取原生命令列表对象 */
	inline ID3D12CommandList* GetD3D12CommandList() const { return m_commandList.Get(); }

	/** 获取原生命令分配器对象 */
	inline ID3D12CommandAllocator* GetD3D12CommandAllocator() const { return m_commandAllocator.Get(); }

	/** 是否正在录制 */
	bool IsRecording() const { return m_isRecording; }
};