#pragma once

#include "RHI/RHIDefinitions.h"

// Forward declarations
class IRHITexture;
class IRHIPiplineState;
class IRHIBuffer;

/**
 * RHI命令列表接口
 */
class IRHICommandList
{
public:
	virtual ~IRHICommandList() = default;

	// -----------------------------------------
	// 渲染命令

	/**
	 * 清除渲染目标
	 *
	 * @param renderTarget 目标渲染纹理
	 * @param clearColor 清除颜色，RGBA格式
	 */
	virtual void ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor) = 0;

	/**
	 * 清除深度模板缓冲区
	 *
	 * @param renderTarget 目标渲染纹理
	 * @param depth 深度值，范围[0.0, 1.0]
	 * @param stencil 模板值，范围[0, 255]
	 */
	virtual void ClearDepthStencil(IRHITexture* renderTarget, float depth, uint8_t stencil) = 0;

	/**
	 * 设置渲染目标和深度模板缓冲区
	 *
	 * @param numRenderTargets 渲染目标数量
	 * @param renderTargets 渲染目标数组
	 * @param depthStencil 深度模板缓冲区
	 */
	virtual void SetRenderTargets(uint32_t numRenderTargets, IRHITexture* const* renderTargets, IRHITexture* depthStencil) = 0;

	/**
	 * 设置视口
	 *
	 * @param x 视口左上角X坐标
	 * @param y 视口左上角Y坐标
	 * @param width 视口宽度
	 * @param height 视口高度
	 * @param minDepth 视口最小深度，范围[0.0, 1.0]
	 * @param maxDepth 视口最大深度，范围[0.0, 1.0]
	 */
	virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

	/**
	 * 设置裁剪矩形
	 *
	 * @param left 裁剪矩形左边界
	 * @param top 裁剪矩形上边界
	 * @param right 裁剪矩形右边界
	 * @param bottom 裁剪矩形下边界
	 */
	virtual void SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom) = 0;

	// -----------------------------------------
	// 资源绑定

	/**
	 * 设置图形管线状态对象
	 *
	 * @param pipelineState 图形管线状态对象句柄
	 */
	virtual void SetPipelineState(RHIHandler pipelineState) = 0;

	/**
	 * 设置顶点缓冲区
	 *
	 * @param slot 顶点缓冲区插槽
	 * @param buffer 顶点缓冲区句柄
	 */
	virtual void SetVertexBuffer(uint32_t slot, IRHIBuffer* buffer) = 0;

	/**
	 * 设置索引缓冲区
	 *
	 * @param buffer 索引缓冲区句柄
	 */
	virtual void SetIndexBuffer(IRHIBuffer* buffer) = 0;

	// -----------------------------------------
	// 绘制命令

	/**
	 * 绘制非索引实例化几何体
	 *
	 * @param vertexCount 每个实例的顶点数量
	 * @param instanceCount 实例数量
	 * @param startVertex 起始顶点偏移
	 * @param startInstance 起始实例偏移
	 */
	virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) = 0;

	/**
	 * 绘制索引实例化几何体
	 *
	 * @param indexCount 每个实例的索引数量
	 * @param instanceCount 实例数量
	 * @param startIndex 起始索引偏移
	 * @param baseVertex 顶点偏移量
	 * @param startInstance 起始实例偏移
	 */
	virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) = 0;

	/** 资源屏障 */
	virtual void ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t starteAfter) = 0;

	/** 获取原生命令列表句柄 */
	virtual RHIHandler GetNativeHandle() const = 0;
};