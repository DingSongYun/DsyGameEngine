#pragma once

#include "RHI/RHIDefinitions.h"

// Forward declarations
class IRHITexture;
class IRHIPiplineState;
class IRHIBuffer;

/**
 * RHI�����б�ӿ�
 */
class IRHICommandList
{
public:
	virtual ~IRHICommandList() = default;

	// -----------------------------------------
	// ��Ⱦ����

	/**
	 * �����ȾĿ��
	 *
	 * @param renderTarget Ŀ����Ⱦ����
	 * @param clearColor �����ɫ��RGBA��ʽ
	 */
	virtual void ClearRenderTarget(IRHITexture* renderTarget, const float* clearColor) = 0;

	/**
	 * ������ģ�建����
	 *
	 * @param renderTarget Ŀ����Ⱦ����
	 * @param depth ���ֵ����Χ[0.0, 1.0]
	 * @param stencil ģ��ֵ����Χ[0, 255]
	 */
	virtual void ClearDepthStencil(IRHITexture* renderTarget, float depth, uint8_t stencil) = 0;

	/**
	 * ������ȾĿ������ģ�建����
	 *
	 * @param numRenderTargets ��ȾĿ������
	 * @param renderTargets ��ȾĿ������
	 * @param depthStencil ���ģ�建����
	 */
	virtual void SetRenderTargets(uint32_t numRenderTargets, IRHITexture* const* renderTargets, IRHITexture* depthStencil) = 0;

	/**
	 * �����ӿ�
	 *
	 * @param x �ӿ����Ͻ�X����
	 * @param y �ӿ����Ͻ�Y����
	 * @param width �ӿڿ��
	 * @param height �ӿڸ߶�
	 * @param minDepth �ӿ���С��ȣ���Χ[0.0, 1.0]
	 * @param maxDepth �ӿ������ȣ���Χ[0.0, 1.0]
	 */
	virtual void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) = 0;

	/**
	 * ���òü�����
	 *
	 * @param left �ü�������߽�
	 * @param top �ü������ϱ߽�
	 * @param right �ü������ұ߽�
	 * @param bottom �ü������±߽�
	 */
	virtual void SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom) = 0;

	// -----------------------------------------
	// ��Դ��

	/**
	 * ����ͼ�ι���״̬����
	 *
	 * @param pipelineState ͼ�ι���״̬������
	 */
	virtual void SetPipelineState(RHIHandler pipelineState) = 0;

	/**
	 * ���ö��㻺����
	 *
	 * @param slot ���㻺�������
	 * @param buffer ���㻺�������
	 */
	virtual void SetVertexBuffer(uint32_t slot, IRHIBuffer* buffer) = 0;

	/**
	 * ��������������
	 *
	 * @param buffer �������������
	 */
	virtual void SetIndexBuffer(IRHIBuffer* buffer) = 0;

	// -----------------------------------------
	// ��������

	/**
	 * ���Ʒ�����ʵ����������
	 *
	 * @param vertexCount ÿ��ʵ���Ķ�������
	 * @param instanceCount ʵ������
	 * @param startVertex ��ʼ����ƫ��
	 * @param startInstance ��ʼʵ��ƫ��
	 */
	virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance) = 0;

	/**
	 * ��������ʵ����������
	 *
	 * @param indexCount ÿ��ʵ������������
	 * @param instanceCount ʵ������
	 * @param startIndex ��ʼ����ƫ��
	 * @param baseVertex ����ƫ����
	 * @param startInstance ��ʼʵ��ƫ��
	 */
	virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance) = 0;

	/** ��Դ���� */
	virtual void ResourceBarrier(IRHITexture* resource, uint32_t stateBefore, uint32_t starteAfter) = 0;

	/** ��ȡԭ�������б��� */
	virtual RHIHandler GetNativeHandle() const = 0;
};