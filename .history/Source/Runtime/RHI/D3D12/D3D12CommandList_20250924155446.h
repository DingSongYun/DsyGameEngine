#pragma once

#include "RHI/RHICommandList.h"
#include "D3D12Common.h"

class D3D12Device;
class IRHITexture;
class IRHIBuffer;
class IRHIPipelineState;

/**
 * @brief D3D12命令列表实现
 */
class D3D12CommandList : public IRHICommandList
{
private:
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	D3D12Device* m_device = nullptr;
	D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	bool m_isRecording = false;

public:
	D3D12CommandList() = default;
	virtual ~D3D12CommandList();

	/**
	 * @brief 初始化命令列表
	 * @param device D3D12设备
	 * @param type 命令列表类型
	 * @return 初始化是否成功
	 */
	bool Initialize(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
	
	/**
	 * @brief 关闭命令列表
	 */
	void Shutdown() override;

	// IRHICommandList 接口实现
	bool Reset() override;
	bool Close() override;

	void ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor) override;
	void ClearDepthStencil(IRHITexture* depthStencil, float depth, uint8_t stencil) override;
	void SetRenderTargets(uint32_t numRenderTargets, IRHITexture** renderTargets, IRHITexture* depthStencil) override;
	void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) override;
	void SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom) override;

	void SetPipelineState(IRHIPipelineState* pipelineState) override;
	void SetVertexBuffer(uint32_t slot, IRHIBuffer* vertexBuffer) override;
	void SetIndexBuffer(IRHIBuffer* indexBuffer) override;

	void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) override;
	void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) override;

	void ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t stateAfter) override;

	RHIHandler GetNativeHandle() const override { return m_commandList.Get(); }

	// D3D12特定接口
	ID3D12GraphicsCommandList* GetD3D12CommandList() const { return m_commandList.Get(); }
	ID3D12CommandAllocator* GetD3D12CommandAllocator() const { return m_commandAllocator.Get(); }
	bool IsRecording() const { return m_isRecording; }
};