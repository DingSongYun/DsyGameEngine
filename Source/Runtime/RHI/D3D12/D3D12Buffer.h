#pragma once

#include "RHI/RHIBuffer.h"
#include "D3D12Common.h"


// Forward declaration
class D3D12Device;

/**
 * D3D12Bufferʵ��
 */
class D3D12Buffer : public IRHIBuffer
{
private:
	// D3D12��Դ
	ComPtr<ID3D12Resource> m_Resource;

	// ��Դ������
	D3D12_RESOURCE_DESC m_ResourceDesc;

	// ��ǰ��Դ״̬
	D3D12_RESOURCE_STATES m_CurrentState = D3D12_RESOURCE_STATE_COMMON;

	// ��Դ����
	RHIBufferDesc m_Desc = {};

	// �Ƿ��ѳ�ʼ��
	bool m_IsInitialized = false;

	// *********************************************
	// ӳ�����
	void* m_MappedData = nullptr;
	bool m_IsMapped = false;

	//**********************************************
	// ��ͼ���

	// ���㻺������ͼ
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
	// ������������ͼ
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
	// ������������ͼ������
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_ConstantBufferViewDesc = {};

public:
	D3D12Buffer() = default;
	virtual ~D3D12Buffer() override;

	bool Initialize(D3D12Device* device, const RHIBufferDesc& desc);
	virtual void Shutdown();

	// ******************************************************
	// IRHIBuffer �ӿ�ʵ��
	virtual uint32_t GetSize() const override { return m_Desc.Size; }
	virtual uint32_t GetStride() const override { return m_Desc.Stride; }
	virtual ERHIBufferUsage GetUsage() const override { return m_Desc.Usage; }
	virtual ERHIBufferAccess GetAccess() const override { return m_Desc.Access; }
	virtual bool Map(RHIBufferMappedData& mappedData) override;
	virtual void Unmap() override;
	virtual bool UpdateData(const void* data, uint32_t size, uint32_t offset = 0) override;
	virtual RHIHandler GetNativeHandle() const override { return m_Resource.Get(); }

	// ******************************************************
	// D3D12Buffer���нӿ�
	ID3D12Resource* GetD3D12Resource() const { return m_Resource.Get(); }
	D3D12_RESOURCE_STATES GetCurrentState() const { return m_CurrentState; }
	void SetCurrentState(D3D12_RESOURCE_STATES state) { m_CurrentState = state; }

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return m_VertexBufferView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return m_IndexBufferView; }
	const D3D12_CONSTANT_BUFFER_VIEW_DESC& GetConstantBufferViewDesc() const { return m_ConstantBufferViewDesc; }

	bool IsValid() const { return m_IsInitialized && m_Resource != nullptr; }

private:
	D3D12_RESOURCE_DESC CreateResourceDesc(const RHIBufferDesc& desc);
	D3D12_HEAP_PROPERTIES GetHeapProperties(const RHIBufferDesc& desc);
	D3D12_RESOURCE_STATES GetInitialResourceState(const RHIBufferDesc& desc);
	D3D12_RESOURCE_FLAGS GetResourceFlags(const RHIBufferDesc& desc);
	void CreateViews(const RHIBufferDesc& desc);
};