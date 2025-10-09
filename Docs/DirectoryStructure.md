# DsyGameEngine 目录结构设计

## 概述

本文档详细描述了DsyGameEngine的目录结构设计，基于模块化架构原则，将引擎代码按功能层次进行组织，确保代码的可维护性、可扩展性和清晰的依赖关系。

## 整体目录结构

```
DsyGameEngine/
├── Source/                         # 源代码目录
│   ├── Runtime/                    # 运行时核心代码
│   ├── Applications/               # 应用程序
│   ├── Examples/                   # 示例项目
│   └── ThirdParty/                 # 第三方库
├── Content/                        # 内容资源
├── Tools/                          # 开发工具
├── Tests/                          # 测试代码
└── Docs/                          # 文档
```

## Runtime 模块详细结构

Runtime目录包含引擎的核心运行时代码，按照架构设计的5层结构组织：

### Core 核心层
```
Runtime/Core/
├── Memory/                         # 内存管理系统
│   ├── MemoryManager.h/.cpp        # 内存管理器
│   ├── Allocators/                 # 各种分配器实现
│   │   ├── PoolAllocator.h/.cpp    # 对象池分配器
│   │   ├── StackAllocator.h/.cpp   # 栈式分配器
│   │   └── FreeListAllocator.h/.cpp # 自由列表分配器
│   └── SmartPointers.h             # 智能指针定义
├── Logging/                        # 日志系统
│   ├── Logger.h/.cpp               # 日志器实现
│   ├── LogLevel.h                  # 日志级别定义
│   └── Formatters/                 # 日志格式化器
├── Config/                         # 配置系统
│   ├── ConfigManager.h/.cpp        # 配置管理器
│   └── ConfigFile.h/.cpp           # 配置文件处理
├── Events/                         # 事件系统
│   ├── Event.h                     # 事件基类
│   ├── EventDispatcher.h/.cpp      # 事件分发器
│   └── EventTypes.h                # 事件类型定义
├── Module/                         # 模块系统
│   ├── IModule.h                   # 模块接口
│   ├── ModuleManager.h/.cpp        # 模块管理器
│   └── Engine.h/.cpp               # 引擎核心类
├── Math/                           # 数学库
│   ├── Vector.h                    # 向量类
│   ├── Matrix.h                    # 矩阵类
│   ├── Quaternion.h                # 四元数类
│   └── MathUtils.h                 # 数学工具函数
├── Containers/                     # 容器类
│   ├── Array.h                     # 动态数组
│   ├── String.h                    # 字符串类
│   └── HashMap.h                   # 哈希表
└── CoreMinimal.h                   # 核心头文件
```

### Platform 平台层
```
Runtime/Platform/
├── Generic/                        # 通用平台接口
│   ├── IPlatform.h                 # 平台接口
│   ├── IWindow.h                   # 窗口接口
│   ├── IInput.h                    # 输入接口
│   └── IFileSystem.h               # 文件系统接口
├── Windows/                        # Windows平台实现
│   ├── WindowsPlatform.h/.cpp      # Windows平台实现
│   ├── WindowsWindow.h/.cpp        # Windows窗口实现
│   ├── WindowsInput.h/.cpp         # Windows输入实现
│   └── WindowsFileSystem.h/.cpp    # Windows文件系统实现
└── FileSystem/                     # 文件系统抽象
    ├── File.h/.cpp                 # 文件操作
    ├── Directory.h/.cpp            # 目录操作
    └── Path.h/.cpp                 # 路径处理
```

### RHI 渲染硬件接口层
```
Runtime/RHI/
├── RHIDefinitions.h                # RHI类型定义
├── RHIDevice.h                     # 设备抽象接口
├── RHICommandList.h                # 命令列表抽象接口
├── RHIResources.h                  # 资源抽象接口
├── RHIBuffer.h                     # 缓冲区抽象接口
├── RHITexture.h                    # 纹理抽象接口
├── RHIShader.h                     # 着色器抽象接口
└── DX12/                           # DirectX 12具体实现
    ├── DX12Device.h/.cpp           # DX12设备实现
    ├── DX12CommandList.h/.cpp      # DX12命令列表实现
    ├── DX12Resources.h/.cpp        # DX12资源实现
    ├── DX12Buffer.h/.cpp           # DX12缓冲区实现
    ├── DX12Texture.h/.cpp          # DX12纹理实现
    ├── DX12Shader.h/.cpp           # DX12着色器实现
    └── DX12Utils.h/.cpp            # DX12工具函数
```

### Renderer 渲染层
```
Runtime/Renderer/
├── RenderCore/                     # 渲染核心
│   ├── Renderer.h/.cpp             # 主渲染器
│   ├── RenderThread.h/.cpp         # 渲染线程
│   ├── RenderCommands.h/.cpp       # 渲染命令
│   └── RenderContext.h/.cpp        # 渲染上下文
├── Resources/                      # 渲染资源
│   ├── Mesh.h/.cpp                 # 网格资源
│   ├── Material.h/.cpp             # 材质资源
│   ├── Texture.h/.cpp              # 纹理资源
│   ├── Shader.h/.cpp               # 着色器资源
│   └── RenderTarget.h/.cpp         # 渲染目标
├── Pipeline/                       # 渲染管线
│   ├── RenderPipeline.h/.cpp       # 渲染管线基类
│   ├── ForwardRenderer.h/.cpp      # 前向渲染器
│   ├── DeferredRenderer.h/.cpp     # 延迟渲染器
│   └── PostProcess.h/.cpp          # 后处理
└── Scene/                          # 场景渲染
    ├── SceneRenderer.h/.cpp        # 场景渲染器
    ├── Camera.h/.cpp               # 摄像机
    ├── Light.h/.cpp                # 光源
    └── RenderProxy.h/.cpp          # 渲染代理
```

### AssetSystem 资源层
```
Runtime/AssetSystem/
├── AssetManager.h/.cpp             # 资源管理器
├── AssetLoader.h/.cpp              # 资源加载器
├── AssetRegistry.h/.cpp            # 资源注册表
├── AssetTypes/                     # 各种资源类型
│   ├── MeshAsset.h/.cpp            # 网格资源
│   ├── TextureAsset.h/.cpp         # 纹理资源
│   ├── MaterialAsset.h/.cpp        # 材质资源
│   ├── ShaderAsset.h/.cpp          # 着色器资源
│   └── SceneAsset.h/.cpp           # 场景资源
├── Loaders/                        # 资源加载器
│   ├── MeshLoader.h/.cpp           # 网格加载器
│   ├── TextureLoader.h/.cpp        # 纹理加载器
│   └── MaterialLoader.h/.cpp       # 材质加载器
└── Serialization/                  # 序列化系统
    ├── Serializer.h/.cpp           # 序列化器
    ├── BinarySerializer.h/.cpp     # 二进制序列化器
    └── JsonSerializer.h/.cpp       # JSON序列化器
```

### GameFramework 游戏逻辑层
```
Runtime/GameFramework/
├── GameObject/                     # 游戏对象系统
│   ├── GameObject.h/.cpp           # 游戏对象基类
│   ├── Component.h/.cpp            # 组件基类
│   ├── Transform.h/.cpp            # 变换组件
│   └── Components/                 # 各种组件
│       ├── RenderComponent.h/.cpp  # 渲染组件
│       ├── CameraComponent.h/.cpp  # 摄像机组件
│       └── LightComponent.h/.cpp   # 光源组件
├── World/                          # 世界/场景系统
│   ├── World.h/.cpp                # 世界类
│   ├── Level.h/.cpp                # 关卡类
│   ├── Scene.h/.cpp                # 场景类
│   └── SceneManager.h/.cpp         # 场景管理器
├── Physics/                        # 物理系统
│   ├── PhysicsWorld.h/.cpp         # 物理世界
│   ├── RigidBody.h/.cpp            # 刚体
│   └── Collider.h/.cpp             # 碰撞器
└── Audio/                          # 音频系统
    ├── AudioEngine.h/.cpp          # 音频引擎
    ├── AudioSource.h/.cpp          # 音频源
    └── AudioListener.h/.cpp        # 音频监听器
```

## Applications 应用程序结构

```
Applications/
├── Engine/                         # 引擎应用
│   ├── EngineApp.h/.cpp            # 引擎应用主类
│   └── main.cpp                    # 程序入口点
├── Editor/                         # 编辑器（未来扩展）
│   └── EditorApp.h/.cpp            # 编辑器应用
└── Game/                           # 游戏应用
    └── GameApp.h/.cpp              # 游戏应用基类
```

## Examples 示例项目结构

```
Examples/
├── EmptyApp/                       # 空应用示例
│   ├── EmptyApplication.cpp        # 空应用实现
│   └── CMakeLists.txt              # 构建配置
├── SimpleTriangle/                 # 简单三角形示例
│   ├── TriangleApp.cpp             # 三角形应用
│   ├── triangle.vs                 # 顶点着色器
│   ├── triangle.ps                 # 像素着色器
│   └── CMakeLists.txt              # 构建配置
└── BasicRenderer/                  # 基础渲染示例
    ├── BasicRendererApp.cpp        # 基础渲染应用
    ├── Shaders/                    # 着色器文件
    └── CMakeLists.txt              # 构建配置
```

## Content 内容资源结构

```
Content/
├── Shaders/                        # 着色器文件
│   ├── Common/                     # 通用着色器
│   │   ├── Common.hlsli            # 通用头文件
│   │   └── Lighting.hlsli          # 光照函数
│   ├── Forward/                    # 前向渲染着色器
│   │   ├── ForwardVS.hlsl          # 前向顶点着色器
│   │   └── ForwardPS.hlsl          # 前向像素着色器
│   └── Deferred/                   # 延迟渲染着色器
│       ├── GBufferVS.hlsl          # GBuffer顶点着色器
│       ├── GBufferPS.hlsl          # GBuffer像素着色器
│       └── LightingPS.hlsl         # 光照像素着色器
├── Textures/                       # 纹理资源
│   ├── Default/                    # 默认纹理
│   ├── UI/                         # UI纹理
│   └── Environment/                # 环境纹理
├── Models/                         # 模型资源
│   ├── Primitives/                 # 基础几何体
│   └── Characters/                 # 角色模型
└── Materials/                      # 材质定义
    ├── Default.mat                 # 默认材质
    └── PBR.mat                     # PBR材质
```

## Tools 开发工具结构

```
Tools/
├── ShaderCompiler/                 # 着色器编译工具
│   ├── ShaderCompiler.cpp          # 编译器主程序
│   └── CompilerConfig.json         # 编译配置
├── AssetProcessor/                 # 资源处理工具
│   ├── AssetProcessor.cpp          # 资源处理器
│   └── ProcessorConfig.json        # 处理配置
└── BuildScripts/                   # 构建脚本
    ├── BuildEngine.bat             # 引擎构建脚本
    └── PackageAssets.bat           # 资源打包脚本
```

## Tests 测试代码结构

```
Tests/
├── UnitTests/                      # 单元测试
│   ├── CoreTests/                  # 核心模块测试
│   ├── RendererTests/              # 渲染器测试
│   └── AssetTests/                 # 资源系统测试
├── IntegrationTests/               # 集成测试
│   ├── RenderingTests/             # 渲染集成测试
│   └── GameFrameworkTests/         # 游戏框架集成测试
└── PerformanceTests/               # 性能测试
    ├── MemoryTests/                # 内存性能测试
    └── RenderingTests/             # 渲染性能测试
```

## 设计原则

### 1. 分层架构
- **明确的依赖关系**：上层依赖下层，下层不依赖上层
- **接口抽象**：每层都提供清晰的接口定义
- **模块独立**：每个模块可以独立开发和测试

### 2. 平台抽象
- **统一接口**：Platform层提供统一的平台接口
- **具体实现**：各平台提供具体的实现
- **易于扩展**：便于后续支持新平台

### 3. 渲染抽象
- **RHI层**：提供渲染硬件接口抽象
- **多后端支持**：便于后续支持Vulkan等其他图形API
- **高级封装**：Renderer层提供高级渲染功能

### 4. 资源管理
- **统一管理**：AssetSystem统一管理所有资源
- **异步加载**：支持异步资源加载
- **热重载**：支持资源热重载功能

### 5. 游戏框架
- **组件系统**：基于组件的游戏对象系统
- **场景管理**：完整的场景和世界管理
- **系统集成**：集成物理、音频等系统

## CMake 组织

每个主要模块都有自己的CMakeLists.txt文件：

```
Source/
├── CMakeLists.txt                  # 主构建文件
├── Runtime/
│   ├── CMakeLists.txt              # Runtime模块构建
│   ├── Core/CMakeLists.txt         # Core子模块构建
│   ├── Platform/CMakeLists.txt     # Platform子模块构建
│   ├── RHI/CMakeLists.txt          # RHI子模块构建
│   ├── Renderer/CMakeLists.txt     # Renderer子模块构建
│   ├── AssetSystem/CMakeLists.txt  # AssetSystem子模块构建
│   └── GameFramework/CMakeLists.txt # GameFramework子模块构建
├── Applications/CMakeLists.txt     # Applications构建
└── Examples/CMakeLists.txt         # Examples构建
```

这种目录结构设计确保了：
- **清晰的模块边界**
- **良好的可维护性**
- **便于团队协作**
- **支持增量构建**
- **易于测试和调试**