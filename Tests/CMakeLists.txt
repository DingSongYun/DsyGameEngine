# Tests build configuration
cmake_minimum_required(VERSION 3.20)

# Enable testing and fetch GoogleTest
include(CTest)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Paths
set(ENGINE_RUNTIME_DIR "${PROJECT_SOURCE_DIR}/Runtime")
# Compute repo root (one level above Source)
get_filename_component(REPO_ROOT "${PROJECT_SOURCE_DIR}/.." ABSOLUTE)

# Test sources
set(TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ShaderCompileTests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/D3D12DeviceTests.cpp
)

# Engine sources needed by tests (avoid refactoring by compiling required .cpp files directly)
set(ENGINE_SOURCES_FOR_TESTS
  ${ENGINE_RUNTIME_DIR}/Render/ShaderCompileUtils.cpp
  ${ENGINE_RUNTIME_DIR}/Core/Logging/Logger.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12Common.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12Device.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12CommandQueue.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12CommandList.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12Fence.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12Texture.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12Buffer.cpp
  ${ENGINE_RUNTIME_DIR}/RHI/D3D12/D3D12SwapChain.cpp
)

add_executable(DsyEngineTests
  ${TEST_SOURCES}
  ${ENGINE_SOURCES_FOR_TESTS}
)

# Include directories (match Runtime target's layout)
target_include_directories(DsyEngineTests PRIVATE
  ${ENGINE_RUNTIME_DIR}
  ${ENGINE_RUNTIME_DIR}/Core
)

# Compile definitions similar to Runtime target
if(WIN32)
  target_compile_definitions(DsyEngineTests PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
  )
endif()

# Link system libraries needed by engine pieces
if(WIN32)
  target_link_libraries(DsyEngineTests PRIVATE
    gtest
    gtest_main
    d3d12
    dxgi
    d3dcompiler
  )
else()
  target_link_libraries(DsyEngineTests PRIVATE gtest gtest_main)
endif()

# Pass repo root to tests to locate shader files
# Note: forward slashes work fine on Windows paths for most Win32 APIs
string(REPLACE "\\" "/" REPO_ROOT_SLASHES "${REPO_ROOT}")
message(STATUS "Repo root detected for tests: ${REPO_ROOT_SLASHES}")
target_compile_definitions(DsyEngineTests PRIVATE DE_REPO_ROOT="${REPO_ROOT_SLASHES}")

# Discover and register tests
set_property(TARGET DsyEngineTests PROPERTY FOLDER "Tests")
gtest_discover_tests(DsyEngineTests)

if(MSVC)
	add_compile_options("/utf-8")
endif()