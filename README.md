# **JUSTOS**

### **注意**

JUSTOS 目前仍处于开发阶段，底层接口和待实现的功能还有很多需要完善。部分模块可能尚未经过充分测试，API 和接口设计可能会随着开发进展进行调整。请开发者在使用过程中注意以下几点：
- 底层接口可能存在不稳定性，建议在开发中保持灵活性以应对潜在的变更。
- 待实现的功能较多，尤其是复杂算法和高级功能模块，可能需要进一步优化和验证。
- 如果您在使用过程中发现问题或有改进建议，欢迎提交 Issue 或 Pull Request，帮助我们一起完善 JUSTOS！

**JUSTOS** 是一个专为嵌入式平台设计的电控开发框架，旨在为机器人、无人机、自动化设备等应用场景提供高效、灵活的软件解决方案。通过模块化设计和硬件抽象层的支持，JUSTOS 能够显著降低开发难度，提升开发效率。

**JUSTOS** 起源于 **JUSTFW (Just FrameWork)**——江理乘风战队电控框架。  
在继承 JUSTFW 核心设计理念的基础上，JUSTOS 进一步优化了系统架构，增强了功能模块的灵活性和可扩展性，以满足更复杂的应用场景需求。

## **核心特性**
1. **TinyBus 机制**  
   在应用底层，JUSTOS 继续采用 JUSTFW 的 TinyBus 机制，实现应用层数据解耦。通过消息总线的方式，简化模块间的通信，提升系统的模块化程度。

2. **流管理**  
   在 TinyBus 底层引入流管理机制，专门用于处理 USB、串口等外设的不定长度数据问题。流管理的主要优势包括：
   - **高效的数据流处理**：能够轻松解析和处理复杂的数据流，降低开发难度。
   - **优化中断性能**：通过将数据流处理从中断上下文中移出，减少了中断处理时间，提升了系统的实时性和稳定性。

3. **框架结构优化**  
   JUSTOS 对 JUSTFW 的框架结构进行了全面优化，主要体现在以下方面：
   - **系统接口与模块接口分级管理**：  
     - 系统接口（`sys/interface`）专注于提供底层硬件抽象和系统级服务（如存储、通信、任务调度等）。  
     - 模块接口（`modules/`）则面向具体功能模块（如电机控制、传感器驱动等），确保模块间独立开发与测试。  
     - 分级管理的设计使得系统层次更加清晰，降低了模块间的耦合度，提升了代码的可维护性和复用性。

4. **FreeRTOS 特性支持**  
   JUSTOS 将逐步引入更多的 FreeRTOS 特性，以支持顶层复杂逻辑的实现。例如，任务调度、信号量、队列等功能将进一步优化，满足多任务并发的需求。

5. **新增功能模块**  
   JUSTOS 增加了以下功能模块，进一步丰富了框架的能力：
   - **easyFlash**：  
     提供非易失性存储管理功能，应用可以通过模块接口轻松实现数据固化，支持配置参数、运行状态等数据的持久化存储。
   - **FreeRTOS CLI**：  
     集成了 FreeRTOS 的命令行接口（CLI），方便开发者通过串口或其他通信方式调试系统。应用层可以通过模块接口快速集成 CLI 功能，实现命令解析和交互。

---

## **JUSTOS 与 JUSTFW 对比**

| 特性            | JUSTOS                          | JUSTFW                          |
|-----------------|---------------------------------|---------------------------------|
| **复杂程度**    | 更高，适合复杂应用场景          | 较低，适合简单应用场景          |
| **可移植性**    | 较低，依赖更多 FreeRTOS 特性    | 更高，轻量化设计，易于移植      |
| **功能扩展性**  | 更强，支持复杂逻辑和多任务处理  | 较弱，主要面向基础功能          |
| **框架结构**    | 上层应用、系统接口与模块接口分级管理      | 单一接口设计，模块耦合度较高    |
| **新增功能**    | FreeRTOS-CLI  Stream     |                         |

---

## **项目结构**

以下是 JUSTOS 的顶层目录及其主要功能说明：

```
JUSTOS/
│
├─CMakeLists.txt                # 全局 CMake 配置文件，用于构建整个项目
├─README.md                     # 全局文档入口，包含项目的概述和快速入门指南
│
├─app/                          # 应用层代码，存放与具体业务逻辑相关的实现
│
├─config/                       # 集中管理配置文件，包括系统级和板级支持包配置
│
├─docs/                         # 文档目录，存放项目的详细设计文档和 API 文档
│
├─interface/                    # 接口定义，提供硬件或软件模块的抽象接口
│
├─lib/                          # 通用库，包含算法、工具和其他可复用的功能模块
│
├─modules/                      # 功能模块，包含具体的硬件或软件功能实现（如电机控制）
│
├─sys/                          # 系统级代码，包括 BSP 和核心框架（如 CLI、存储管理等）
│
├─examples/                     # 示例项目，提供快速上手的代码示例
│
└─tests/                        # 测试代码，用于单元测试和集成测试
```
----
以下是补全后的 TODO 列表，结合了你的需求和常见的嵌入式系统开发任务，进一步完善了内容结构，并补充了一些可能遗漏的任务项。希望对你有所帮助！

---

# **TODO**

## **文档完善**
- [ ] 完善 DOC readme 文档
  - 添加项目概述、架构设计说明
  - 补充外设封装模块的使用指南
  - 提供器件驱动的详细接口说明
  - 更新 CLI 命令的使用示例

---

## **外设封装**
- [ ] bsp_can
  - CAN 故障预警与重置
- [ ] bsp_uart
- [ ] bsp_i2c
- [ ] bsp_usb
  - USB CLI
  - USB 视觉通信
  - USB 实时调试
- [ ] bsp_spi
  - 实现 SPI 主从模式通信
  - 支持多片选管理
- [ ] bsp_gpio
  - 实现 GPIO 输入输出配置
  - 支持中断触发模式
- [ ] bsp_pwm
  - 实现 PWM 输出配置
  - 支持频率与占空比动态调整
- [ ] bsp_adc
  - 实现 ADC 数据采集
  - 支持多通道扫描模式
- [ ] 蜂鸣器
  - 蜂鸣器开机音效
  - 蜂鸣器

## **系统核心**
- [ ] TinyBus 接口类 FreeRTOS 风格封装
  - 实现消息队列、订阅发布机制
  - 提供线程安全的总线通信接口
- [ ] Stream 接口类 FreeRTOS 风格封装
  - 实现流数据缓冲区管理
  - 支持流数据的异步读写操作
- [ ] 新增日志存储模块
- 实现日志的环形缓冲区管理，支持日志的异步写入与读取。
- 提供线程安全的日志接口，确保多任务环境下的日志记录可靠性。
- 支持日志分级（如 DEBUG、INFO、WARN、ERROR）。
- 提供日志导出功能，支持通过 CLI 或文件传输方式导出日志。
---
## **器件驱动**
### **电机驱动**
- [ ] C610
- [ ] C620
- [ ] GM6020
- [ ] MF9025
- [ ] CyberGear
- [ ] 通用 PWM
- [ ] 通信步进电机驱动
- SteadyWin
  - [x] SteadyWin通用MIT通信接口
  - [ ] SteadyWin

### **IMU 驱动**
- [ ] MPU6600
  - 实现加速度计与陀螺仪数据读取
  - 提供姿态解算接口
- [ ] BMI088
  - 实现高精度惯性测量
  - 支持温度补偿功能
- [ ] IST8310
  - 实现磁力计数据读取
  - 提供磁场校准功能

### **遥控器驱动**
- [ ] DR16
  - 实现遥控信号解析
  - 提供按键与摇杆状态反馈
- [ ] 蜂鸣器
  - 实现音调与频率控制
  - 简单音乐播放功能
  - 支持异常报警
- [ ] 裁判系统解析
  - 实现裁判系统数据包解析

---

## **CLI 部分**
### **env 环境变量管理 CLI**
- [x] 支持环境变量的查看、设置和删除
- [x] 提供默认环境变量初始化功能
- [x] 支持环境变量持久化存储

### **motor 电机管理 CLI**
- [X] 支持电机状态查询（如速度、位置、电流等）
- [ ] 统一电机底层配置接口（PID控制参数、MIT控制参数）
- [ ] 支持电机参数配置（如 PID 参数、运行模式等）
- [ ] 提供电机启停控制接口

### **stream 流管理 CLI**
- [ ] 支持流数据的实时监控
- [ ] 提供流缓冲区状态查询功能

### **tinybus BUS 总线管理 CLI**
- [ ] 支持总线消息的订阅与发布测试
- [ ] 提供总线节点状态查询功能

### **Log 日志管理 CLI**
- [ ] 日志查看
- [ ] 日志下载

### **新增文件传输 CLI**
- [ ] **文件传输模块**
  - 支持通过 CLI 进行文件上传与下载操作。
  - 提供断点续传功能，确保大文件传输的可靠性。
  - 支持传输进度显示与传输日志记录。
  - 日志内容包括文件名、传输方向（上传/下载）、传输时间、传输结果等。

---

## **其他任务**
- [ ] 基于宏定义和自定义脚本兼容C板
- [ ] 单元测试框架搭建
  - 使用 Unity 或 CMock 实现单元测试
  - 提供覆盖率分析工具
- [ ] 性能优化
  - 优化关键路径代码执行效率
  - 减少内存占用与功耗
- [ ] 错误处理
  - 实现统一的错误码管理

## **致谢**
特别感谢参与 JUSFW 开发的学长学姐。  
感谢所有为 JUSTFW, JUSTOS 贡献代码、提出建议和支持的开发者和用户！您的支持是我们不断前进的动力。