#pragma once

#include "RHIDefinitions.h"

/**
 * ������ʹ������
 */
enum class ERHIBufferUsage
{
	None = 0,
	VertexBuffer = 1 << 0,			// ���㻺����, ���ڴ洢��������, ��λ�á����ߡ����������, ����Ⱦ����ʹ��, ���ڶ��弸�������״������, �������Ρ��߶ε�
	IndexBuffer = 1 << 1,			// ����������, ���ڴ洢��������, �Ա����ö�������, �����ȾЧ��, ���ڶ��弸��������˽ṹ�� ���������б��߶��б��
	ConstantBuffer = 1 << 2,		// ����������, ���ڴ洢��ɫ����������, ��任���󡢹��ղ�����, ����ɫ������Ⱦ������ʹ��, ���ڴ��ݲ�Ƶ���仯��ȫ������
	ShaderResource = 1 << 3,		// ��ɫ����Դ������, ֻ��, ���ڴ洢�����������ݵ�, ����ɫ������Ⱦ�����ж�ȡ, ���ڴ���Ƶ���仯����Դ����
	UnorderedAccess = 1 << 4,		// ������ʻ�����, �ɶ�д, ���ڴ洢������ɫ�����������, ��������ɫ���ڼ�������ж�д, ����ʵ�ָ��ӵļ�������ݴ���
	Staging = 1 << 5,				// ��ʱ������������CPU-GPU���ݴ��䣩, ������CPU��GPU֮�䴫������, ��CPUд������, Ȼ��GPU��ȡ����, ����GPUд������, Ȼ��CPU��ȡ����
	CopySource = 1 << 6,			// ����Դ������, �������ݴ��������Դ������, �����ݴӸû���������Դ���䵽��һ��������, �����ڸ��Ʋ���
	CopyDestination = 1 << 7,		// ����Ŀ�껺����, �������ݴ��������Ŀ�껺����, �����ݴ��䵽�û���������Դ, �����ڸ��Ʋ���
	Dynamic = 1 << 8,				// ��̬������, ����Ƶ���������ݵĻ�����, ��CPUƵ��д������, Ȼ��GPU��ȡ����, ��������ҪƵ�����µĳ���, ������ϵͳ����̬�������
};

/**
 * ����������Ȩ��
 */
enum class ERHIBufferAccess
{
	None = 0,
	CPURead = 1 << 0,					// CPU��ȡ���������ݵ�Ȩ��
	CPUWrite = 1 << 1,					// CPUд�뻺�������ݵ�Ȩ��
	GPURead = 1 << 2,					// GPU��ȡ���������ݵ�Ȩ��
	GPUWrite = 1 << 3					// GPUд�뻺�������ݵ�Ȩ��
};


/**
 * ������������
 */
struct RHIBufferDesc
{
	uint32_t Size = 0;									// ��������С, ��λ: �ֽ�
	uint32_t Stride = 0;								// ������Ԫ�ز���(ÿ��Ԫ�صĴ�С), ��λ: �ֽ�
	ERHIBufferUsage Usage = ERHIBufferUsage::None;		// ������ʹ������, �ο�BufferUsageö��
	ERHIBufferAccess Access = ERHIBufferAccess::None;	// ����������Ȩ��, �ο�BufferAccessö��
	ERHIFormat Format = ERHIFormat::UNKNOWN;			// ��������ʽ, �ο�Formatö��
	uint32_t MiscFlags = 0;								// ������־, �ο�BufferMiscFlagö��
};


/**
 * ������ӳ������
 * ��������ӳ�䵽CPU��ַ�ռ�Ļ���������
 * ʹ�ó���:
 *	 1. CPU��Ҫֱ�ӷ���GPU����������ʱ, ������¶������ݡ���ȡ��������
 *	 2. ʵ�ָ�Ч�����ݴ���ʹ���, ���ⲻ��Ҫ���ڴ濽��
 *	 3. ��Ҫ�����ӵ����ݲ���, ���������ݵ��ж���Ͳ����
 * ע������:
 *	 1. ӳ�����ͨ����Ҫ�뻺�����Ĵ�����ʹ�÷�ʽ��ƥ��, ��ȷ������һ���Ժ���ȷ��
 *	 2. ӳ��������ָ����ʹ����Ϻ�, ��Ҫ��ʱ���ӳ��, ���ͷ���Դ
 *	 3. ӳ��������ܻ�Ӱ������, Ӧ���ݾ��������ʹ�ó�������Ȩ��
 */
struct RHIBufferMappedData
{
	void* Data = nullptr;		// ָ��ӳ�����ݵ�ָ��
	uint32_t Size = 0;			// ӳ�����ݵ��ܴ�С, ��λ: �ֽ�
	uint32_t RowPitch = 0;		// ÿ�����ݵ��ֽ���, ��Ҫ������������
	uint32_t DepthPitch = 0;	// ÿ�����ݵ��ֽ���, ��Ҫ����3D��������
};

class IRHIBuffer
{
public:
	virtual ~IRHIBuffer() = default;

	// ����������
	virtual uint32_t GetSize() const = 0;
	virtual uint32_t GetStride() const = 0;
	virtual ERHIBufferUsage GetUsage() const = 0;
	virtual ERHIBufferAccess GetAccess() const = 0;

	// ���ݲ���
	virtual bool Map(RHIBufferMappedData& mappedData) = 0;
	virtual void Unmap() = 0;
	virtual bool UpdateData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

	// ��ȡԭ�����
	virtual RHIHandler GetNativeHandle() const = 0;
};