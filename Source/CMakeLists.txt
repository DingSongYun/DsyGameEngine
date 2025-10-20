cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 启用C++20模块支持
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# 项目配置
project(DsyGameEngine
	VERSION 1.0.0
	LANGUAGES CXX
)

# 编译器特性检查
if(MSVC)
	# Visual Studio 2019 16.11+ 或 Visual Studio 2022
	if(MSVC_VERSION LESS 1929)
		message(FATAL_ERROR "Visual Studio 2019 16.11 or later is required for C++20 support")
	endif()

	# MSVC编译选项
	add_compile_options(/W4)  # 警告级别
	add_compile_options(/permissive-)  # 严格标准符合性
	add_compile_options(/Zc:__cplusplus)  # 正确的__cplusplus宏值
	add_compile_options(/experimental:module) # 启用模块支持
endif()


# include
include_directories("${PROJECT_SOURCE_DIR}/")
include_directories("${PROJECT_SOURCE_DIR}/Runtime")

# 
add_subdirectory(Runtime)

# Include test suite
add_subdirectory(${PROJECT_SOURCE_DIR}/../Tests ${PROJECT_BINARY_DIR}/Tests)