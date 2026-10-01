# 重构验收与未验证项

软件构建、主机算法/协议检查和静态检查结果见 validation/。它们没有替代硬件回归。以下均为重构基线验收项，不是已经开始开发的功能 TODO：

| 项目 | 验收动作 | 验收条件/验证方法 |
|---|---|---|
| 所有板载工程 | 确认芯片型号、探针、供电、引脚和固件 | ST-Link/ICDI 读取型号；烧录 verify；保存板卡照片/版本和结果 |
| STM32 | 同版 CubeMX 修改 IO 后重新生成 | Drivers/Middlewares 未升级；USER CODE 桥接、DMA 和 FreeRTOS 设置保留；重新完整 Debug/Release 构建 |
| maoxiu | 电机/编码器/电压基线 | 悬空或台架逐轮测方向、计数和比例；低压、断串口、非法命令输出归零；与旧基线对照 |
| TM4C | 移植后的运动标定 | 实测轮径/减速比/QEI 窗口、轨距、ADC 分压及 PI；CALIBRATED 保持 0，完成台架验收后才配置；不假填参数 |
| 星璇 | 标定、滤波、解算、PID 与混控 | 无桨台架确认采样→解算→控制→输出；实测 5ms 控制/解算、失联/旧数据保护、温控方向；回放与旧实现对照，再单独进行飞行验收 |
| 所有 RTOS | 时序、栈、队列和资源 | 逻辑分析仪记录周期/抖动/最坏执行时间；uxTaskGetStackHighWaterMark、空闲堆、队列满与创建失败注入；确认 IRQ 可调用 FromISR 的优先级 |
| 所有板载工程 | 探针烧录与故障恢复 | verify/reset 后锁定安全状态；DMA错误、传感器掉线、总线超时、看门狗复位回归并登记实际测量 |
| ROS | Linux ROS2 全工程构建与实机链路 | 按原 ROS 发行版安装依赖并 colcon build；验证 odom/IMU、电压和 cmd_vel 的协议 v1；本机仅运行纯 Python 协议测试 |

STM32 固定生成结构；当前验证了所有 Core 文件 USER CODE 外模板与原提交一致、厂商与 RTOS 源码字节不变，但尚未运行 CubeMX GUI 重生成。GCC 10.3.1 平台端口来自与 STM32 内核匹配的官方标签，位于 firmware/os/ports，原 RVDS 端口仍随原内核保留。TM4C 参考仓库没有对应 maoxiu RTOS 分支，因此该新分支显式新增 FreeRTOS 10.5.1，而不是升级原仓库内核。

AR 根据用户确认作为存档：海思 SDK 完整编译和上板验证不在本次执行范围；GN 路径及主机模型/消息测试已检查。Wi-Fi/lwIP/CMSIS 接口适配仍须在原 SDK 中复核。MQTT 接收原先关闭，本次继续关闭；不增加重连、页面或 OTA。
