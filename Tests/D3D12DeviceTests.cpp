#include <gtest/gtest.h>
#include "RHI/D3D12/D3D12Device.h"
#include "RHI/RHIDevice.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIBuffer.h"

TEST(D3D12Device, InitializeAndShutdown) {
  D3D12Device device;
  ASSERT_TRUE(device.Initialize());
  EXPECT_TRUE(device.IsValid());

  EXPECT_NE(device.GetCommandQueue(ERHICommandQueueType::Graphics), nullptr);
  EXPECT_NE(device.GetCommandQueue(ERHICommandQueueType::Compute), nullptr);
  EXPECT_NE(device.GetCommandQueue(ERHICommandQueueType::Copy), nullptr);

  device.Shutdown();
  EXPECT_FALSE(device.IsValid());
}

TEST(D3D12Device, CreateRenderTargetTexture) {
  D3D12Device device;
  ASSERT_TRUE(device.Initialize());

  RHITextureDesc desc{};
  desc.width = 64;
  desc.height = 64;
  desc.format = ERHIFormat::R8G8B8A8_UNORM;
  desc.usageFlags = static_cast<uint32_t>(ERHITextureUsage::RenderTarget) | static_cast<uint32_t>(ERHITextureUsage::ShaderResource);

  auto tex = device.CreateTexture(desc);
  ASSERT_NE(tex, nullptr);
  EXPECT_EQ(tex->GetWidth(), 64);
  EXPECT_EQ(tex->GetHeight(), 64);

  device.Shutdown();
}

TEST(D3D12Device, CreateVertexBuffer) {
  D3D12Device device;
  ASSERT_TRUE(device.Initialize());

  RHIBufferDesc desc{};
  desc.Size = 256;
  desc.Stride = sizeof(float);
  desc.Usage = ERHIBufferUsage::VertexBuffer;
  desc.Access = ERHIBufferAccess::GPURead;
  desc.Format = ERHIFormat::UNKNOWN;

  auto buf = device.CreateBuffer(desc);
  ASSERT_NE(buf, nullptr);
  EXPECT_EQ(buf->GetSize(), 256);
  EXPECT_EQ(buf->GetStride(), sizeof(float));

  device.Shutdown();
}