#include <stdio.h>
#include <concepts>
#include <ranges>
#include <format>  // 如果编译器支持
#include <span>
#include <vector>
#include "Logging/Logger.h"

#include "RHI/D3D12/D3D12Device.h"
#include "RHI/D3D12/D3D12CommandQueue.h"
#include "RHI/RHIDevice.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIBuffer.h"


// 概念测试
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 范围测试
void testRanges() {
	std::vector<int> numbers = {1, 2, 3, 4, 5};
	auto even = numbers | std::views::filter([](int n) { return n % 3 == 0; });
}

void TestRenderSimpleTriangle()
{
	D3D12Device device;
	device.Initialize();

	RHIBufferDesc desc{};
	desc.Size = 256;
	desc.Stride = sizeof(float);
	desc.Usage = ERHIBufferUsage::VertexBuffer;
	desc.Access = ERHIBufferAccess::GPURead;
	desc.Format = ERHIFormat::UNKNOWN;

	auto buf = device.CreateBuffer(desc);

	device.Shutdown();
}

int main()
{
	int ret = 0;

	printf("lalala\n");
	testRanges();
	LOG_INFO("111 Hello, World");
	LOG_DEBUG("222 Hello, {}!", "World");
	LOG_ERROR("333 Hello, {}!", "World");
	TestRenderSimpleTriangle();
	//DE_LOG(LogLevel::ERROR, "Hello, {}!", "World");
	// if ((ret = GApp->StartupModule()) != 0)
	// {
	// 	printf("App Start failed, Exit!!!");
	// 	return ret;
	// }

	// while(!GApp->IsQuit())
	// {
	// 	GApp->Tick();
	// }

	// GApp->ShutdownModule();

	return ret;
}