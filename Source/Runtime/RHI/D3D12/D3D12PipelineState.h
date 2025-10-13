#pragma once

#include "D3D12Common.h"

enum class D3D12PipelineType
{
	Graphics,
	Compute,
	RayTracing
};

// 图形管线描述符
struct D3D12GraphicsPipelineDesc
{
	// ***********************************
	// 必填
	ComPtr<ID3D12RootSignature>		RootSignature;									// 根签名
	D3D12_SHADER_BYTECODE			VS{};											// 顶点着色器, Vertex Shader
	D3D12_SHADER_BYTECODE			PS{};											// 像素着色器, Pixel Shader

	// ***********************************
	// 选填
	D3D12_SHADER_BYTECODE			GS{};											// 几何着色器, Geometry Shader 
	D3D12_SHADER_BYTECODE			HS{};											// 细分控制着色器，Hull Shader
	D3D12_SHADER_BYTECODE			DS{};											// 细分域着色器, Domain Shader
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;								// 输入布局, 作用于定义顶点数据的格式和结构
	CD3DX12_BLEND_DESC				BlendState{ CD3DX12_DEFAULT {} };				// 混合状态, 作用于定义颜色混合的方式
	CD3DX12_RASTERIZER_DESC			RasterizerState{ CD3DX12_DEFAULT {} };			// 光栅化状态, 作用于定义如何将几何图形转换为像素
	CD3DX12_DEPTH_STENCIL_DESC1		DepthStencilState{ CD3DX12_DEFAULT {} };		// 深度模板状态, 作用于定义深度测试和模板测试的方式
	ERHIFormat						DSVFormat = ERHIFormat::UNKNOWN;				// 深度模板格式
	D3D12_RT_FORMAT_ARRAY			RTVFormats{};									// 渲染目标格式数组
	DXGI_SAMPLE_DESC				SampleDesc{ 1, 0 };								// 多重采样描述
	uint32_t 						SampleMask = UINT_MAX;							// 样本遮罩, 作用于定义哪些样本可以被写入
	D3D12_PIPELINE_STATE_FLAGS		Flags = D3D12_PIPELINE_STATE_FLAG_NONE;			// 管线状态标志
	D3D12_CACHED_PIPELINE_STATE		CachedPSO{};									// 缓存的管线状态对象

	// IA(Input Assembler) 拓扑类型 (PSO中保存的是TopologyType, 真正的PrimitiveTopology需要在命令列表里设置)
	D3D12_PRIMITIVE_TOPOLOGY_TYPE	PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	std::wstring					DebugName;										// 调试名
};


// 计算管线描述符
struct D3D12ComputePipelineDesc
{
	// ***********************************
	// 必填
	ComPtr<ID3D12RootSignature>		RootSignature;									// 根签名
	D3D12_SHADER_BYTECODE			CS{};											// 着色器

	// ***********************************
	// 选填
	D3D12_PIPELINE_STATE_FLAGS		Flags = D3D12_PIPELINE_STATE_FLAG_NONE;			// 管线状态标志
	D3D12_CACHED_PIPELINE_STATE		CachedPSO{};									// 缓存的管线状态对象
	std::wstring					DebugName;										// 调试名

};

// 光线追踪管线描述符
struct D3D12RayTracingPipelineDesc
{
	// TODO: 实现光线追踪管线状态描述符
};

class D3D12PipelineState
{
private:
	// 管线状态对象
	ComPtr<ID3D12PipelineState>		m_PSO;
	// 管线类型
	D3D12PipelineType				m_Type = D3D12PipelineType::Graphics;
public:
	D3D12PipelineState() = default;
	virtual ~D3D12PipelineState();

	/** 创建图形PSO */
	bool InitializeGraphics(ID3D12Device* device, const D3D12GraphicsPipelineDesc& desc);

	/** 创建计算PSO */
	bool InitializeCompute(ID3D12Device* device, const D3D12ComputePipelineDesc& desc);

	void Shutdown();

	inline RHIHandler GetNativeHandle () const { return m_PSO.Get(); }
	inline ID3D12PipelineState* Get() const { return m_PSO.Get(); }
	inline D3D12PipelineType GetType() const { return m_Type; }
};