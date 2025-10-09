#pragma once

#include "RHI/RHITexture.h"
#include "D3D12Common.h"


// Forward declaration
class D3D12Device;


/**
 * D3D12����ʵ��
 */
class D3D12Texture : public IRHITexture
{
private:
	// D3D12��Դ����
	ComPtr<ID3D12Resource>	m_texture;

	// ��ǰ��Դ״̬
	D3D12_RESOURCE_STATES	m_currentState = D3D12_RESOURCE_STATE_COMMON;

	// D3D12��Դ������
	D3D12_RESOURCE_DESC		m_ResourceDesc = {};


	// ----------------------------------------------
	// ������Դ��ͼ���

	// RTV: Render Target View ��ȾĿ����ͼ���
	// ������Ϊ��ȾĿ��ʱʹ��
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle = {};

	// DSV: Depth Stencil View ���ģ����ͼ���
	// ������Ϊ���ģ��ʱʹ��
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle = {};

	// SRV: Shader Resource View ��ɫ����Դ��ͼ���
	// ������Ϊֻ����Դ����ɫ������
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle = {};

	// UAV: Unordered Access View ���������ͼ���
	// ������ɫ����������������д����
	// һ�����ڼ�����ɫ�������Ŀ��
	// ʹ�ó���: ������ɫ������������Ⱦ������ϵͳ��
	D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle = {};

	bool m_IsInitialized = false;

public:
	D3D12Texture() = default;
	virtual ~D3D12Texture() override;

	bool Initialize(D3D12Device* device, const RHITextureDesc& desc);
	bool InitializeFromReource(D3D12Device* device, ID3D12Resource* resource);
	virtual void Shutdown();

	// ------------------------------------------------------
	// IRHITexture�ӿ�ʵ��
	virtual uint64_t GetWidth() const override { return m_ResourceDesc.Width; }
	virtual uint64_t GetHeight() const override { return m_ResourceDesc.Height; }
	virtual uint32_t GetDepth() const override;
	virtual uint32_t GetMipLevels() const override { return m_ResourceDesc.MipLevels; }
	virtual ERHIFormat GetFormat() const override;
	virtual RHIHandler GetNativeHandle() const override { return m_texture.Get(); }

	// ------------------------------------------------------
	// D3D12Texture���нӿ�

	inline ID3D12Resource* GetD3D12Resource() const { return m_texture.Get(); }
	inline D3D12_RESOURCE_STATES GetCurrentState() const { return m_currentState; }
	inline void SetCurrentState(D3D12_RESOURCE_STATES state) { m_currentState = state; }

	// �������������
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const { return m_RTVHandle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return m_DSVHandle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const { return m_SRVHandle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() const { return m_UAVHandle; }

	inline void SetRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_RTVHandle = handle; }
	inline void SetDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_DSVHandle = handle; }
	inline void SetSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_SRVHandle = handle; }
	inline void SetUAVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_UAVHandle = handle; }

private:
	D3D12_RESOURCE_DESC CreateResourceDesc(const RHITextureDesc& desc) const;
	D3D12_HEAP_PROPERTIES GetHeapProperties() const;
	D3D12_RESOURCE_STATES GetInitialResourceState(uint32_t usageFlags) const;
	D3D12_RESOURCE_FLAGS GetResourceFlags(uint32_t usageFlags) const;
};
