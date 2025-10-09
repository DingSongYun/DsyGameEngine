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

	// ������Դ������
	m_ResourceDesc = CreateResourceDesc(desc);

	// ��ȡ������
	D3D12_HEAP_PROPERTIES heapProps = GetHeapProperties();

	// ��ȡ��ʼ״̬����Դ��־
	m_currentState = GetInitialResourceState(desc.usageFlags);

	// ����������Դ
	HRESULT hr = device->GetD3D12Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&m_ResourceDesc,
		m_currentState,
		nullptr, // ��ʼ���ֵ
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
	m_currentState = D3D12_RESOURCE_STATE_PRESENT; // TODO: ����ʵ��ʹ�ó������ó�ʼ״̬

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
	// �������������
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
	// ����ά��
	if( desc.depth > 1)
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; // 3D����
	}
	else if (desc.arraySize > 1)
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // ��������
	}
	else
	{
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; // ��ͨ����
	}

	resourceDesc.Alignment = 0; // Ĭ�϶���
	resourceDesc.Width = desc.width;
	resourceDesc.Height = desc.height;
	resourceDesc.DepthOrArraySize = static_cast<uint16_t>(desc.depth > 1 ? desc.depth : desc.arraySize);
	resourceDesc.MipLevels = static_cast<uint16_t>(desc.mipLevels);
	resourceDesc.Format = D3D12Utils::ConvertFormat(desc.format);
	resourceDesc.SampleDesc.Count = desc.sampleCount; // Ĭ�ϵ�����
	resourceDesc.SampleDesc.Quality = desc.sampleQuality;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // Ĭ�ϲ���
	resourceDesc.Flags = GetResourceFlags(desc.usageFlags);

	return resourceDesc;
}

D3D12_HEAP_PROPERTIES D3D12Texture::GetHeapProperties() const
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT; // Ĭ�϶����ͣ��ʺ�GPU����
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;
	return heapProps;
}

D3D12_RESOURCE_STATES D3D12Texture::GetInitialResourceState(uint32_t usageFlags) const
{
	// ����ʹ�ñ�־ȷ����ʼ��Դ״̬
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
	return D3D12_RESOURCE_STATE_COMMON; // Ĭ��״̬
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

	// �������������ɫ����Դ����������DENY_SHADER_RESOURCE��־���Ż�����
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
	return 1; // ����2D��1D�������Ϊ1
}
