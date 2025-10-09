# DsyGameEngine

一个基于DirectX 12和C++20的现代游戏引擎，用于个人学习和理解游戏引擎架构。

## 项目特点

- **现代C++20**: 充分利用C++20的新特性（模块、协程、概念、范围等）
- **DirectX 12**: 基于DX12实现高性能渲染
- **模块化架构**: 清晰的分层设计，便于学习和扩展
- **跨平台抽象**: 设计了平台抽象层，便于后续扩展
- **RHI抽象**: 渲染硬件接口抽象，便于支持多种图形API

## 目录结构

```
DsyGameEngine/
├── Source/                         # 源代码目录
│   ├── Runtime/                    # 运行时核心代码
│   │   ├── Core/                   # 核心层（内存、日志、模块系统等）
│   │   ├── Platform/               # 平台抽象层
│   │   ├── RHI/                    # 渲染硬件接口层
│   │   ├── Renderer/               # 渲染层
│   │   ├── AssetSystem/            # 资源管理层
│   │   └── GameFramework/          # 游戏逻辑层
│   ├── Applications/               # 应用程序
│   ├── Examples/                   # 示例项目
│   └── ThirdParty/                 # 第三方库
├── Content/                        # 内容资源（着色器、纹理、模型等）
├── Tools/                          # 开发工具
├── Tests/                          # 测试代码
└── Docs/                          # 项目文档
```

详细的目录结构说明请参考：[目录结构设计文档](Docs/DirectoryStructure.md)

## 架构设计

引擎采用分层模块化架构：

1. **核心层 (Core)**: 提供内存管理、日志系统、配置系统、事件系统等基础功能
2. **平台层 (Platform)**: 处理平台相关的窗口管理、输入处理、文件系统等
3. **RHI层**: 渲染硬件接口抽象，目前实现DX12后端
4. **渲染层 (Renderer)**: 高级渲染功能，包括渲染管线、场景渲染等
5. **资源层 (AssetSystem)**: 统一的资源管理和加载系统
6. **游戏逻辑层 (GameFramework)**: 游戏对象系统、场景管理、物理系统等

## 文档

- [架构设计](Docs/ArchitectureDesign.md) - 整体架构设计说明
- [核心模块](Docs/CoreModules.md) - 核心模块详细设计
- [渲染系统](Docs/RenderingSystem.md) - 渲染系统架构说明
- [开发计划](Docs/DevelopmentPlan.md) - 开发路线图和计划
- [目录结构](Docs/DirectoryStructure.md) - 详细的目录结构说明

## 开发环境

- **编译器**: Visual Studio 2022 (支持C++20)
- **图形API**: DirectX 12
- **构建系统**: CMake 3.20+
- **平台**: Windows 10/11 (x64)

## 构建说明

### 使用构建脚本（推荐）

项目提供了统一的构建脚本 `build_and_run.bat`，支持多种操作：

```bash
# 构建Debug版本（默认）
build_and_run.bat

# 构建Release版本
build_and_run.bat build Release

# 运行程序（会自动构建如果需要）
build_and_run.bat run

# 运行Release版本
build_and_run.bat run Release

# 清理构建目录
build_and_run.bat clean

# 重新构建
build_and_run.bat rebuild

# 显示帮助信息
build_and_run.bat help
```

### 手动构建

如果需要手动构建，可以使用以下命令：

```bash
# 进入Source目录
cd Source

# 生成Visual Studio项目文件
cmake -B build -S . -G "Visual Studio 17 2022" -A x64

# 编译Debug版本
cmake --build build --config Debug

# 编译Release版本
cmake --build build --config Release
```

## 学习目标

通过开发这个游戏引擎，学习和理解：

- 现代游戏引擎架构设计
- DirectX 12图形编程
- C++20新特性的实际应用
- 内存管理和性能优化
- 资源管理和异步加载
- 渲染管线和图形技术
- 游戏对象系统设计

## 开发阶段

当前处于第一阶段：**核心基础设施搭建**

- [x] 架构设计文档
- [x] 目录结构设计
- [x] C++20环境配置
- [x] 构建系统配置
- [x] 统一构建脚本
- [ ] 核心模块实现
- [ ] 基础日志系统
- [ ] 内存管理系统
- [ ] 模块系统框架

详细的开发计划请参考：[开发计划文档](Docs/DevelopmentPlan.md)