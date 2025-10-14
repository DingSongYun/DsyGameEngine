#include "D3D12Texture.h"
#include "D3D12Device.h"

D3D12Texture::~D3D12Texture()
{
	Shutdown();
}

bool D3D12Texture::Initialize(D3D12Device* device, const RHITextureDesc& desc)
{
	if (!device)
	{
		LOG_INFO("D3D12Texture::Initialize: device is null");
		return false;
	}

	DE_ASSERT(m_IsInitialized);

	// 创建资源描述符
	m_ResourceDesc = CreateResourceDesc(desc);

	// 获取堆属性
	D3D12_HEAP_PROPERTIES heapProps = GetHeapProperties();

	// 获取初始状态和资源标志
	m_currentState = GetInitialResourceState(desc.usageFlags);

	// 创建纹理资源
	HRESULT hr = device->GetD3D12Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&m_ResourceDesc,
		m_currentState,
		nullptr, // 初始清除值
		IID_PPV_ARGS(&m_texture)
	);

	if (FAILED(hr))
	{
		LOG_ERROR("D3D12Texture::Initialize: Failed to create texture resource. HRESULT: 0x%X", hr);
		return false;
	}

	m_IsInitialized = true;
	LOG_INFO("D3D12Texture::Initialize: Texture resource created successfully.");
	return true;
}

bool D3D12Texture::InitializeFromReource(D3D12Device* device, ID3D12Resource* resource)
{
	if (!device || !resource)
	{
		LOG_INFO("D3D12Texture::InitializeFromReource: device or resource is null");
		return false;
	}

	DE_ASSERT(m_IsInitialized);

	m_texture = resource;
	m_ResourceDesc = resource->GetDesc();
	m_currentState = D3D12_RESOURCE_STATE_PRESENT; // TODO: 根据实际使用场景设置初始状态

	m_IsInitialized = true;
	LOG_INFO("D3D12Texture::InitializeFromReource: Texture resource assigned successfully.");
	return true;
}

void D3D12Texture::Shutdown()
{
	if (m_texture)
	{
		m_texture.Reset();
	}
	// 重置描述符句柄
	m_RTVHandle = {};
	m_DSVHandle = {};
	m_SRVHandle = {};
	m_UAVHandle = {};
	m_IsInitialized = false;
	m_currentState = D3D12_RESOURCE_STATE_COMMON;
}

D3D12_RESOURCE_DESC D3D12Texture::CreateResourceDesc(const RHITextureDesc& desc) const
{
	D3D12_RESOURCE_DESC resourceDesc = {};
	// 设置维度
	if( desc.depth > 1)
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; // 3D纹理
	}
	else if (desc.arraySize > 1)
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 数组纹理
	}
	else
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; // 普通纹理
	}

	resourceDesc.Alignment = 0; // 默认对齐
	resourceDesc.Width = desc.width;
	resourceDesc.Height = desc.height;
	resourceDesc.DepthOrArraySize = static_cast<uint16_t>(desc.depth > 1 ? desc.depth : desc.arraySize);
	resourceDesc.MipLevels = static_cast<uint16_t>(desc.mipLevels);
	resourceDesc.Format = D3D12Utils::ConvertFormat(desc.format);
	resourceDesc.SampleDesc.Count = desc.sampleCount; // 默认单采样
	resourceDesc.SampleDesc.Quality = desc.sampleQuality;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // 默认布局
	resourceDesc.Flags = GetResourceFlags(desc.usageFlags);

	return resourceDesc;
}

D3D12_HEAP_PROPERTIES D3D12Texture::GetHeapProperties() const
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT; // 默认堆类型，适合GPU访问
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;
	return heapProps;
}

D3D12_RESOURCE_STATES D3D12Texture::GetInitialResourceState(uint32_t usageFlags) const
{
	// 根据使用标志确定初始资源状态
	if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::RenderTarget))
	{
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	}
	else if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::DepthStencil))
	{
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}
	else if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::ShaderResource))
	{
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	}
	else if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::UnorderedAccess))
	{
		return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}
	else if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::CopyDestination))
	{
		return D3D12_RESOURCE_STATE_COPY_DEST;
	}
	else if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::CopySource))
	{
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	}
	return D3D12_RESOURCE_STATE_COMMON; // 默认状态
}

D3D12_RESOURCE_FLAGS D3D12Texture::GetResourceFlags(uint32_t usageFlags) const
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::RenderTarget))
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::DepthStencil))
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}
	if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::UnorderedAccess))
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	// 如果纹理不用作着色器资源，可以设置DENY_SHADER_RESOURCE标志来优化性能
	if (!(usageFlags & static_cast<uint32_t>(ERHITextureUsage::ShaderResource)))
	{
		if (usageFlags & static_cast<uint32_t>(ERHITextureUsage::DepthStencil))
		{
			flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		}
	}
	return flags;
}

ERHIFormat D3D12Texture::GetFormat() const
{
	return D3D12Utils::ConvertFormat(m_ResourceDesc.Format);
}

uint32_t D3D12Texture::GetDepth() const
{
	if (m_ResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
	{
		return m_ResourceDesc.DepthOrArraySize;
	}
	return 1; // 对于2D和1D纹理，深度为1
}
