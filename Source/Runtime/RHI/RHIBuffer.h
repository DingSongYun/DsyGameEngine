#pragma once

#include "RHIDefinitions.h"

/**
 * 缓冲区使用类型
 */
enum class ERHIBufferUsage
{
	None = 0,
	VertexBuffer = 1 << 0,			// 顶点缓冲区, 用于存储顶点数据, 如位置、法线、纹理坐标等, 供渲染管线使用, 用于定义几何体的形状和属性, 如三角形、线段等
	IndexBuffer = 1 << 1,			// 索引缓冲区, 用于存储索引数据, 以便重用顶点数据, 提高渲染效率, 用于定义几何体的拓扑结构， 如三角形列表、线段列表等
	ConstantBuffer = 1 << 2,		// 常量缓冲区, 用于存储着色器常量数据, 如变换矩阵、光照参数等, 供着色器在渲染过程中使用, 用于传递不频繁变化的全局数据
	ShaderResource = 1 << 3,		// 着色器资源缓冲区, 只读, 用于存储纹理、顶点数据等, 供着色器在渲染过程中读取, 用于传递频繁变化的资源数据
	UnorderedAccess = 1 << 4,		// 无序访问缓冲区, 可读写, 用于存储计算着色器的输出数据, 供计算着色器在计算过程中读写, 用于实现复杂的计算和数据处理
	Staging = 1 << 5,				// 临时缓冲区（用于CPU-GPU数据传输）, 用于在CPU和GPU之间传输数据, 供CPU写入数据, 然后GPU读取数据, 或者GPU写入数据, 然后CPU读取数据
	CopySource = 1 << 6,			// 复制源缓冲区, 用于数据传输操作的源缓冲区, 供数据从该缓冲区或资源传输到另一个缓冲区, 常见于复制操作
	CopyDestination = 1 << 7,		// 复制目标缓冲区, 用于数据传输操作的目标缓冲区, 供数据传输到该缓冲区或资源, 常见于复制操作
	Dynamic = 1 << 8,				// 动态缓冲区, 用于频繁更新数据的缓冲区, 供CPU频繁写入数据, 然后GPU读取数据, 适用于需要频繁更新的场景, 如粒子系统、动态几何体等
};

/**
 * 缓冲区访问权限
 */
enum class ERHIBufferAccess
{
	None = 0,
	CPURead = 1 << 0,					// CPU读取缓冲区数据的权限
	CPUWrite = 1 << 1,					// CPU写入缓冲区数据的权限
	GPURead = 1 << 2,					// GPU读取缓冲区数据的权限
	GPUWrite = 1 << 3					// GPU写入缓冲区数据的权限
};


/**
 * 缓冲区描述符
 */
struct RHIBufferDesc
{
	uint32_t Size = 0;									// 缓冲区大小, 单位: 字节
	uint32_t Stride = 0;								// 缓冲区元素步长(每个元素的大小), 单位: 字节
	ERHIBufferUsage Usage = ERHIBufferUsage::None;		// 缓冲区使用类型, 参考BufferUsage枚举
	ERHIBufferAccess Access = ERHIBufferAccess::None;	// 缓冲区访问权限, 参考BufferAccess枚举
	ERHIFormat Format = ERHIFormat::UNKNOWN;			// 缓冲区格式, 参考Format枚举
	uint32_t MiscFlags = 0;								// 其他标志, 参考BufferMiscFlag枚举
};


/**
 * 缓冲区映射数据
 * 用于描述映射到CPU地址空间的缓冲区数据
 * 使用场景:
 *	 1. CPU需要直接访问GPU缓冲区数据时, 例如更新顶点数据、读取计算结果等
 *	 2. 实现高效的数据传输和处理, 避免不必要的内存拷贝
 *	 3. 需要处理复杂的数据布局, 如纹理数据的行对齐和层对齐
 * 注意事项:
 *	 1. 映射操作通常需要与缓冲区的创建和使用方式相匹配, 以确保数据一致性和正确性
 *	 2. 映射后的数据指针在使用完毕后, 需要及时解除映射, 以释放资源
 *	 3. 映射操作可能会影响性能, 应根据具体需求和使用场景进行权衡
 */
struct RHIBufferMappedData
{
	void* Data = nullptr;		// 指向映射数据的指针
	uint32_t Size = 0;			// 映射数据的总大小, 单位: 字节
	uint32_t RowPitch = 0;		// 每行数据的字节数, 主要用于纹理数据
	uint32_t DepthPitch = 0;	// 每层数据的字节数, 主要用于3D纹理数据
};

class IRHIBuffer
{
public:
	virtual ~IRHIBuffer() = default;

	// 缓冲区属性
	virtual uint32_t GetSize() const = 0;
	virtual uint32_t GetStride() const = 0;
	virtual ERHIBufferUsage GetUsage() const = 0;
	virtual ERHIBufferAccess GetAccess() const = 0;

	// 数据操作
	virtual bool Map(RHIBufferMappedData& mappedData) = 0;
	virtual void Unmap() = 0;
	virtual bool UpdateData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

	// 获取原生句柄
	virtual RHIHandler GetNativeHandle() const = 0;
};