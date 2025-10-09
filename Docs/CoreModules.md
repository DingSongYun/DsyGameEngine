# DsyGameEngine 核心模块设计

## 概述

本文档详细描述了DsyGameEngine的核心模块设计，包括内存管理、资源系统、场景系统、输入系统等关键组件。

## 内存管理系统

### 设计目标

- 高效的内存分配和回收
- 减少内存碎片
- 支持多线程内存操作
- 内存使用追踪和调试

### 核心组件

- **MemoryManager**: 内存管理器，负责全局内存策略
- **MemoryAllocator**: 内存分配器基类
- **PoolAllocator**: 对象池分配器，用于固定大小对象
- **StackAllocator**: 栈式分配器，用于临时分配
- **FreeListAllocator**: 自由列表分配器，用于一般用途

### 智能指针

利用C++20特性，实现增强版智能指针：

```cpp
template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using UniqueRef = std::unique_ptr<T>;
```

## 资源管理系统

### 设计目标

- 统一的资源加载和管理接口
- 资源引用计数和生命周期管理
- 异步资源加载
- 资源热重载

### 核心组件

- **ResourceManager**: 资源管理器，负责资源的注册和查找
- **ResourceLoader**: 资源加载器，负责不同类型资源的加载
- **ResourceCache**: 资源缓存，负责已加载资源的缓存
- **ResourceHandle**: 资源句柄，提供对资源的安全访问

### 资源类型

- **Texture**: 纹理资源
- **Mesh**: 网格资源
- **Material**: 材质资源
- **Shader**: 着色器资源
- **Audio**: 音频资源
- **Config**: 配置资源

### 资源加载流程

1. 请求资源
2. 检查资源缓存
3. 如果缓存命中，返回资源句柄
4. 如果缓存未命中，创建资源加载任务
5. 执行资源加载（同步或异步）
6. 资源处理和转换
7. 缓存资源并返回资源句柄

## 场景系统

### 设计目标

- 高效的场景对象管理
- 灵活的场景图结构
- 支持场景序列化和反序列化
- 场景对象查询和遍历

### 核心组件

- **Scene**: 场景类，管理场景中的所有对象
- **SceneNode**: 场景节点，构成场景图的基本单位
- **GameObject**: 游戏对象，场景中的实体
- **Component**: 组件，附加到游戏对象上提供功能
- **Transform**: 变换组件，定义对象的位置、旋转和缩放

### 场景图

场景采用树形结构组织，每个节点可以有多个子节点：

```
Scene
├── RootNode
│   ├── GameObject1
│   │   ├── TransformComponent
│   │   ├── RenderComponent
│   │   └── PhysicsComponent
│   ├── GameObject2
│   │   ├── TransformComponent
│   │   └── AudioComponent
│   └── GameObject3
│       ├── TransformComponent
│       └── ScriptComponent
└── CameraNode
    └── Camera
        ├── TransformComponent
        └── CameraComponent
```

### 组件系统

组件是游戏对象功能的载体，主要包括：

- **TransformComponent**: 变换组件
- **RenderComponent**: 渲染组件
- **CollisionComponent**: 碰撞组件
- **AudioComponent**: 音频组件
- **ScriptComponent**: 脚本组件

## 输入系统

### 设计目标

- 统一的输入处理接口
- 支持多种输入设备
- 可配置的输入映射
- 输入事件分发

### 核心组件

- **InputManager**: 输入管理器，负责输入设备的管理和事件分发
- **InputDevice**: 输入设备基类
- **Keyboard**: 键盘设备
- **Mouse**: 鼠标设备
- **Gamepad**: 游戏手柄设备
- **InputAction**: 输入动作，将物理输入映射到逻辑动作
- **InputContext**: 输入上下文，定义不同场景下的输入映射

### 输入处理流程

1. 输入设备产生原始输入
2. InputManager接收原始输入
3. 根据当前InputContext进行输入映射
4. 生成InputAction事件
5. 分发InputAction事件到注册的处理器

## 事件系统

### 设计目标

- 低耦合的组件间通信
- 支持同步和异步事件
- 事件优先级和过滤
- 性能优化的事件分发

### 核心组件

- **EventManager**: 事件管理器，负责事件的注册和分发
- **Event**: 事件基类，定义事件的基本属性
- **EventListener**: 事件监听器，接收和处理事件
- **EventDispatcher**: 事件分发器，将事件分发给监听器

### 事件类型

- **SystemEvent**: 系统事件，如窗口大小改变、应用焦点变化等
- **InputEvent**: 输入事件，如按键、鼠标移动等
- **SceneEvent**: 场景事件，如对象创建、销毁等
- **GameplayEvent**: 游戏玩法事件，如得分、生命值变化等

## 任务系统

### 设计目标

- 高效的多线程任务调度
- 任务依赖管理
- 负载均衡
- 与C++20协程集成

### 核心组件

- **TaskManager**: 任务管理器，负责任务的创建和调度
- **Task**: 任务基类，定义任务的基本属性和执行逻辑
- **TaskGraph**: 任务图，描述任务之间的依赖关系
- **ThreadPool**: 线程池，执行任务的线程集合

### 任务类型

- **ComputeTask**: 计算任务，执行CPU密集型计算
- **IOTask**: IO任务，执行文件或网络IO操作
- **RenderTask**: 渲染任务，执行渲染相关操作
- **AsyncTask**: 异步任务，可与协程配合使用

## 反射系统

### 设计目标

- 运行时类型信息
- 序列化和反序列化支持
- 编辑器集成
- 脚本绑定

### 核心组件

- **TypeRegistry**: 类型注册表，管理所有注册的类型
- **TypeInfo**: 类型信息，描述类型的属性和方法
- **PropertyInfo**: 属性信息，描述类型的属性
- **MethodInfo**: 方法信息，描述类型的方法

### 反射使用

```cpp
// 类型注册
REGISTER_CLASS(GameObject)
    .PROPERTY(GameObject, name, String)
    .PROPERTY(GameObject, position, Vector3)
    .METHOD(GameObject, SetPosition, void, const Vector3&)
    .METHOD(GameObject, GetPosition, Vector3);

// 反射使用
auto gameObject = TypeRegistry::CreateInstance<GameObject>();
TypeInfo* typeInfo = TypeRegistry::GetTypeInfo<GameObject>();
PropertyInfo* positionProp = typeInfo->GetProperty("position");
positionProp->SetValue(gameObject, Vector3(1, 2, 3));
```