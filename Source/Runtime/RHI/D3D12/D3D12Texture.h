#pragma once

#include "RHI/RHITexture.h"
#include "D3D12Common.h"


// Forward declaration
class D3D12Device;


/**
 * D3D12纹理实现
 */
class D3D12Texture : public IRHITexture
{
private:
	// D3D12资源对象
	ComPtr<ID3D12Resource>	m_texture;

	// 当前资源状态
	D3D12_RESOURCE_STATES	m_currentState = D3D12_RESOURCE_STATE_COMMON;

	// D3D12资源描述符
	D3D12_RESOURCE_DESC		m_ResourceDesc = {};


	// ----------------------------------------------
	// 纹理资源视图句柄

	// RTV: Render Target View 渲染目标视图句柄
	// 纹理作为渲染目标时使用
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle = {};

	// DSV: Depth Stencil View 深度模板视图句柄
	// 纹理作为深度模板时使用
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle = {};

	// SRV: Shader Resource View 着色器资源视图句柄
	// 纹理作为只读资源供着色器采用
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle = {};

	// UAV: Unordered Access View 无序访问视图句柄
	// 允许着色器对纹理进行随机读写访问
	// 一般用于计算着色器的输出目标
	// 使用场景: 计算着色器输出、体积渲染、粒子系统等
	D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle = {};

	bool m_IsInitialized = false;

public:
	D3D12Texture() = default;
	virtual ~D3D12Texture() override;

	bool Initialize(D3D12Device* device, const RHITextureDesc& desc);
	bool InitializeFromReource(D3D12Device* device, ID3D12Resource* resource);
	virtual void Shutdown();

	// ------------------------------------------------------
	// IRHITexture接口实现
	virtual uint64_t GetWidth() const override { return m_ResourceDesc.Width; }
	virtual uint64_t GetHeight() const override { return m_ResourceDesc.Height; }
	virtual uint32_t GetDepth() const override;
	virtual uint32_t GetMipLevels() const override { return m_ResourceDesc.MipLevels; }
	virtual ERHIFormat GetFormat() const override;
	virtual RHIHandler GetNativeHandle() const override { return m_texture.Get(); }

	// ------------------------------------------------------
	// D3D12Texture特有接口

	inline ID3D12Resource* GetD3D12Resource() const { return m_texture.Get(); }
	inline D3D12_RESOURCE_STATES GetCurrentState() const { return m_currentState; }
	inline void SetCurrentState(D3D12_RESOURCE_STATES state) { m_currentState = state; }

	// 描述符句柄操作
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
