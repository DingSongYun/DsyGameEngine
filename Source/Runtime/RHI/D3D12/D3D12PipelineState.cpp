#include "D3D12PipelineState.h"

D3D12PipelineState::~D3D12PipelineState()
{
	Shutdown();
}

bool D3D12PipelineState::InitializeGraphics(ID3D12Device* device, const D3D12GraphicsPipelineDesc& desc)
{
	if (!device)
	{
		LOG_ERROR("D3D12PipelineState::InitializeGraphics() - Invalid D3D12Device provided.");
		return false;
	}

	if (desc.VS.BytecodeLength == 0 || desc.VS.pShaderBytecode == nullptr)
	{
		LOG_ERROR("D3D12PipelineState::InitializeGraphics() - Vertex shader bytecode is required.");
		return false;
	}

	if (desc.PS.BytecodeLength == 0 || desc.PS.pShaderBytecode == nullptr)
	{
		LOG_ERROR("D3D12PipelineState::InitializeGraphics() - Pixel shader bytecode is required.");
		return false;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
		.pRootSignature		= desc.RootSignature.Get(),
		.VS					= desc.VS,
		.PS					= desc.PS,
		.DS = desc.DS,
		.HS = desc.HS,
		.GS = desc.GS,
		.StreamOutput = {},
		.BlendState = desc.BlendState,
		.SampleMask = desc.SampleMask,
		.RasterizerState = desc.RasterizerState,
		.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(desc.DepthStencilState), // ×ªµ½V0°æ±¾
		.InputLayout = {
			.pInputElementDescs = desc.InputLayout.empty() ? nullptr : desc.InputLayout.data(),
			.NumElements = static_cast<UINT>(desc.InputLayout.size()),
		},
		.PrimitiveTopologyType = desc.PrimitiveTopologyType,
		.NumRenderTargets = desc.RTVFormats.NumRenderTargets,
		.DSVFormat = D3D12Utils::ConvertFormat(desc.DSVFormat),
		.SampleDesc = desc.SampleDesc,
		.CachedPSO = desc.CachedPSO,
		.Flags = desc.Flags,
	};
	for (uint32_t i = 0; i < desc.RTVFormats.NumRenderTargets; ++i)
	{
		psoDesc.RTVFormats[i] = desc.RTVFormats.RTFormats[i];
	}

	HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_PSO.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		m_PSO.Reset();
		LOG_ERROR("D3D12PipelineState::InitializeGraphics() - Failed to create graphics pipeline state. HRESULT: 0x%08X", hr);
		return false;
	}

	m_Type = D3D12PipelineType::Graphics;

	// For Debug
	if (!desc.DebugName.empty() && m_PSO)
	{
		m_PSO->SetName(desc.DebugName.c_str());
	}
	return true;
}

bool D3D12PipelineState::InitializeCompute(ID3D12Device* device, const D3D12ComputePipelineDesc& desc)
{
	if (!device)
	{
		LOG_ERROR("D3D12PipelineState::InitializeGraphics() - Invalid D3D12Device provided.");
		return false;
	}

	if (!desc.RootSignature)
	{
		LOG_ERROR("D3D12PipelineState::InitializeCompute() - RootSignature is required.");
		return false;
	}
	if (desc.CS.pShaderBytecode == nullptr || desc.CS.BytecodeLength == 0)
	{
		LOG_ERROR("D3D12PipelineState::InitializeCompute() - Compute shader bytecode is required.");
		return false;
	}

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = desc.RootSignature.Get();
	psoDesc.CS = desc.CS;
	psoDesc.Flags = desc.Flags;
	psoDesc.CachedPSO = desc.CachedPSO;

	HRESULT hr = device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(m_PSO.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		m_PSO.Reset();
		LOG_ERROR("D3D12PipelineState::InitializeCompute() - Failed to create compute pipeline state. HRESULT: 0x%08X", hr);
		return false;
	}

	m_Type = D3D12PipelineType::Compute;

	if (!desc.DebugName.empty() && m_PSO)
	{
		m_PSO->SetName(desc.DebugName.c_str());
	}

	return true;
}

void D3D12PipelineState::Shutdown()
{
	m_PSO.Reset();
	m_Type = D3D12PipelineType::Graphics;
}
