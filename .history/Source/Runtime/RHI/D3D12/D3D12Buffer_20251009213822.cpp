#include "D3D12Buffer.h"
#include "D3D12Device.h"

D3D12Buffer::~D3D12Buffer()
{
	Shutdown();
}

bool D3D12Buffer::Initialize(D3D12Device* device, const RHIBufferDesc& desc, const void* initialData)
{
	DE_ASSERT(!m_IsInitialized, "D3D12Buffer::Initialize failed: buffer is already initialized");

	if (!device)
	{
		LOG_ERROR("D3D12Buffer::Initialize failed: device is null");
		return false;
	}

	if (desc.Size == 0)
	{
		LOG_ERROR("D3D12Buffer::Initialize failed: buffer size is zero");
		return false;
	}

	m_Desc = desc;

	// ������Դ������
	m_ResourceDesc = CreateResourceDesc(desc);

	// ��ȡ������
	D3D12_HEAP_PROPERTIES heapProps = GetHeapProperties(desc);

	// ��ʼ״̬
	m_CurrentState = GetInitialResourceState(desc);

	// ������������Դ
	HRESULT hr = device->GetD3D12Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&m_ResourceDesc,
		m_CurrentState,
		nullptr,
		IID_PPV_ARGS(&m_Resource)
	);

	if (FAILED(hr))
	{
		LOG_ERROR("D3D12Buffer::Initialize failed: CreateCommittedResource failed, hr = 0x%08X", hr);
		return false;
	}

	// ������ͼ
	CreateViews(desc);
	m_IsInitialized = true;
	LOG_INFO("D3D12Buffer::Initialize success. Size: {} bytes, Usage: {}", desc.Size, static_cast<uint32_t>(desc.Usage));

	return true;
}

void D3D12Buffer::Shutdown()
{
	if(m_IsMapped)
	{
		Unmap();
	}

	if(m_Resource)
	{
		m_Resource.Reset();
	}

	// ������ͼ
	m_VertexBufferView = {};
	m_IndexBufferView = {};
	m_ConstantBufferViewDesc = {};

	// ����
	m_IsInitialized = false;
	m_CurrentState = D3D12_RESOURCE_STATE_COMMON;
}

bool D3D12Buffer::Map(RHIBufferMappedData& mappedData)
{
	DE_ASSERT(m_Resource, "D3D12Buffer::Map failed: resource is null");
	
	if (m_IsMapped)
	{
		LOG_WARN("D3D12Buffer::Map failed: buffer is already mapped");
		mappedData.Data = m_MappedData;
		mappedData.Size = m_Desc.Size;
		return true;
	}

	if (E_UINT32(m_Desc.Access) & (E_UINT32(ERHIBufferAccess::CPUWrite) | E_UINT32(ERHIBufferAccess::CPUWrite)) == 0)
	{
		LOG_ERROR("D3D12Buffer::Map failed: buffer is not CPU accessible");
		return false;
	}

	D3D12_RANGE readRange = {};
	if (!(E_UINT32(m_Desc.Access) & E_UINT32(ERHIBufferAccess::CPURead)))
	{
		// ���û��CPU��ȡȨ��, ����Ҫ��ȡ�κ�����
		readRange.Begin = 0;
		readRange.End = 0;
	}
	else
	{
		// ����, ��ȡ����������
		readRange.Begin = 0;
		readRange.End = m_Desc.Size;
	}

	HRESULT hr = m_Resource->Map(0, &readRange, &m_MappedData);
	if (FAILED(hr))
	{
		LOG_ERROR("D3D12Buffer::Map failed: ID3D12Resource::Map failed, hr = 0x%08X", hr);
		return false;
	}

	m_IsMapped = true;
	mappedData.Data = m_MappedData; // ָ��ӳ�����ݵ�ָ��
	mappedData.Size = m_Desc.Size; // ӳ�����ݵ��ܴ�С
	mappedData.RowPitch = m_Desc.Stride; // ���ڻ�����, �о���ڲ���
	mappedData.DepthPitch = m_Desc.Size; // ���ڻ�����, ��Ⱦ���ڴ�С

	return false;
}

void D3D12Buffer::Unmap()
{
	if (!m_IsMapped || !m_Resource)
	{
		return;
	}

	D3D12_RANGE writeRange = {};
	if (!(E_UINT32(m_Desc.Access) & E_UINT32(ERHIBufferAccess::CPUWrite)))
	{
		// ���û��CPU��ȡȨ��, ����Ҫд���κ�����
		writeRange.Begin = 0;
		writeRange.End = 0;
	}
	else
	{
		// ����, д������������
		writeRange.Begin = 0;
		writeRange.End = m_Desc.Size;
	}

	m_Resource->Unmap(0, &writeRange);
	m_MappedData = nullptr;
	m_IsMapped = false;
}

bool D3D12Buffer::UpdateData(const void* data, uint32_t size, uint32_t offset)
{
	if (!data || size == 0)
	{
		LOG_ERROR("D3D12Buffer::UpdateData failed: invalid data or size");
		return false;
	}

	if (offset + size > m_Desc.Size)
	{
		LOG_ERROR("D3D12Buffer::UpdateData failed: data size exceeds buffer size");
		return false;
	}

	// ���ڶ�̬��������ʹ��ӳ��ķ�ʽ����
	if (E_UINT32(m_Desc.Usage) & E_UINT32(ERHIBufferUsage::Dynamic))
	{
		RHIBufferMappedData mappedData = {};
		if (!Map(mappedData))
		{
			LOG_ERROR("D3D12Buffer::UpdateData failed: Map failed");
			return false;
		}
		// �������ݵ�ӳ����ڴ�
		std::memcpy(static_cast<uint8_t*>(mappedData.Data) + offset, data, size);
		Unmap();
		return true;
	}
	// ���ھ�̬����������Ҫʹ�ø�������
	else
	{
		LOG_WARN("D3D12Buffer::UpdateData() - Static buffer update not fully implemented. Use staging buffer and copy commands.");
		return false;
	}
	return false;
}

D3D12_RESOURCE_DESC D3D12Buffer::CreateResourceDesc(const RHIBufferDesc& inDesc)
{
	return {
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		.Alignment = 0,
		.Width = inDesc.Size,
		.Height = 1,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = {
			.Count = 1,
			.Quality =	0
		},
		.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		.Flags = GetResourceFlags(inDesc)
	};
}

D3D12_HEAP_PROPERTIES D3D12Buffer::GetHeapProperties(const RHIBufferDesc& desc)
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;

	// ���ݷ��ʱ�־ȷ��������
	const bool cpuAccess = (E_UINT32(desc.Access)) 
		& (E_UINT32(ERHIBufferAccess::CPURead) | E_UINT32(ERHIBufferAccess::CPUWrite));
	if (cpuAccess)
	{
		if (E_UINT32(desc.Usage) & E_UINT32(ERHIBufferUsage::Staging))
		{
			heapProps.Type = D3D12_HEAP_TYPE_READBACK;
		}
		else
		{
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		}
	}
	else 
	{
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	}
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	return D3D12_HEAP_PROPERTIES();
}

D3D12_RESOURCE_STATES D3D12Buffer::GetInitialResourceState(const RHIBufferDesc& desc)
{
	uint32_t usageFlag = static_cast<uint32_t>(desc.Usage);
	if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::VertexBuffer))
	{
		return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}
	else if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::IndexBuffer))
	{
		return D3D12_RESOURCE_STATE_INDEX_BUFFER;
	}
	else if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::ConstantBuffer))
	{
		return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}
	else if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::UnorderedAccess))
	{
		return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}
	else if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::CopyDestination))
	{
		return D3D12_RESOURCE_STATE_COPY_DEST;
	}
	else if (usageFlag & static_cast<uint32_t>(ERHIBufferUsage::CopySource))
	{
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	}

	return D3D12_RESOURCE_STATE_COMMON;
}

D3D12_RESOURCE_FLAGS D3D12Buffer::GetResourceFlags(const RHIBufferDesc& desc)
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

	if (E_UINT32(desc.Usage) & E_UINT32(ERHIBufferUsage::UnorderedAccess))
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	return flags;
}

void D3D12Buffer::CreateViews(const RHIBufferDesc& desc)
{
	DE_ASSERT(m_Resource, "D3D12Buffer::CreateViews failed: resource is null");

	const uint32_t usageFlag = E_UINT32(desc.Usage);

	// �������㻺������ͼ
	if (usageFlag & E_UINT32(ERHIBufferUsage::VertexBuffer))
	{
		m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = desc.Size;
		m_VertexBufferView.StrideInBytes = desc.Stride;
	}


	// ����������������ͼ
	if (usageFlag & E_UINT32(ERHIBufferUsage::IndexBuffer))
	{
		m_VertexBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		m_VertexBufferView.SizeInBytes = desc.Size;
		if (desc.Stride == 2)
		{
			m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		}
		else if (desc.Stride == 4)
		{
			m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		}
		else
		{
			LOG_WARN("D3D12Buffer::CreateViews failed: invalid index buffer stride: {}", desc.Stride);
		}
	}

	// ����������������ͼ
	if (usageFlag & E_UINT32(ERHIBufferUsage::ConstantBuffer))
	{
		m_ConstantBufferViewDesc.BufferLocation = m_Resource->GetGPUVirtualAddress();
		// ������������С������256�ֽڶ����
		m_ConstantBufferViewDesc.SizeInBytes = (desc.Size + 255) & ~255;
	}
}
