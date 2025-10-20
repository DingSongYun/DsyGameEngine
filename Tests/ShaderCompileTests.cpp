#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <wrl/client.h>
#include "Render/ShaderCompileUtils.h"

#ifndef DE_REPO_ROOT
#define DE_REPO_ROOT "."
#endif

static std::wstring ToWide(const std::string& s) {
  return std::wstring(s.begin(), s.end());
}

static std::wstring ShaderPath(const std::string& relative) {
  std::filesystem::path p(DE_REPO_ROOT);
  p /= relative;
  return p.wstring();
}

TEST(ShaderCompile, CompilesTriangleVS) {
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  const std::wstring path = ShaderPath("Content/Shaders/TriangleVS.hlsl");
  bool ok = CompileShaderFromFile(path, "VSMain", "vs_5_0", blob.GetAddressOf());
  EXPECT_TRUE(ok);
  EXPECT_NE(blob.Get(), nullptr);
}

TEST(ShaderCompile, CompilesTrianglePS) {
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  const std::wstring path = ShaderPath("Content/Shaders/TrianglePS.hlsl");
  bool ok = CompileShaderFromFile(path, "PSMain", "ps_5_0", blob.GetAddressOf());
  EXPECT_TRUE(ok);
  EXPECT_NE(blob.Get(), nullptr);
}

TEST(ShaderCompile, FailsWithWrongEntryPoint) {
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  const std::wstring path = ShaderPath("Content/Shaders/TriangleVS.hlsl");
  bool ok = CompileShaderFromFile(path, "NotExist", "vs_5_0", blob.GetAddressOf());
  EXPECT_FALSE(ok);
  EXPECT_EQ(blob.Get(), nullptr);
}