#pragma once

#include "RHI/RHISwapChain.h"
#include "D3D12Common.h"

// Forward declarations
class D3D12Device;
class IRHITexture;
struct RHISwapChainDesc;

/**
 * D3D12交换链
 */
class D3D12SwapChain : public IRHISwapChain
{
private:
	/** 交换链 */
	ComPtr<IDXGISwapChain3> m_SwapChain;

	/** buffers */
	std::vector<ComPtr<ID3D12Resource>> m_BackBuffers;

	// 关联的D3D12设备
	D3D12Device* m_Device = nullptr;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_BufferCount = 0;
	ERHIFormat m_Format = ERHIFormat::UNKNOWN;

public:
	D3D12SwapChain() = default;
	virtual ~D3D12SwapChain();

	bool Initialize(D3D12Device* device, const RHISwapChainDesc& desc);
	void Shutdown();

	// --------------------------------------------
	// IRHISwapChain 接口
	virtual uint32_t GetCurrentBackBufferIndex() const override;
	virtual inline uint32_t GetBufferCount() const override { return m_BufferCount; }
	virtual IRHITexture* GetBackBuffer(uint32_t Index) const override;
	virtual void Present(uint32_t SyncInterval = 1) override;
	virtual bool Resize(uint32_t Width, uint32_t Height) override;
	virtual inline uint32_t GetWidth() const override { return m_Width; }
	virtual inline uint32_t GetHeight() const override { return m_Height; }
	virtual inline ERHIFormat GetFormat() const override { return m_Format; }
	virtual inline RHIHandler GetNativeHandle() const override { return m_SwapChain.Get(); }

private:
	// --------------------------------------------
	// D3D12接口
	bool CreateBackBuffers();
	void ReleaseBackBuffers();
};