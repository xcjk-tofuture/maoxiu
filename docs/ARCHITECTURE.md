# StarPleiades（昴宿）架构

入口：`firmware/platform/stm32/startup_stm32f407xx.S: Reset_Handler` → `Core/Src/main.c: main` → `MX_FREERTOS_Init` → `firmware/app/startup.c: app_tasks_init` → `osKernelStart`。
`APP_RTOS_EXTERNAL_TASKS=1` 排除生成模板中的旧任务，实际创建配置以 `startup.c` 为准。

`Core/Drivers/Middlewares/.ioc` 保持 CubeMX 结构。手写代码位于 `firmware/app/services/algorithms/drivers/platform/boards/os`，分别负责业务与任务、服务、独立计算、设备、芯片适配、板级资源和必要同步。
CubeMX 开启 Keep User Code，重新生成后核查初始化桥接、DMA 回调、内核配置、时基和 CMake 源清单，完整构建后再上板。

UART1 接收队列 → 协议解码/分发 → 底盘命令队列 → `Encoder_Task_Proc` → `chassis_step` → 麦轮运动学/PI → 电机接口 → PWM。控制任务拥有闭环状态；遥测和显示复制快照。10 ms 控制、20 ms 正常输出，默认 500 ms 指令超时和 6 V 欠压保护。

## 任务与资源所有权

| 任务 | 优先级 | 配置栈 | 周期/等待 | 资源与失败策略 |
|---|---|---|---|---|
| Control | High | 512 words | 10ms 固定周期，输出 20ms；故障立即停止 | 闭环状态、命令队列4、ADC/编码器/PWM |
| PC | Normal | 768 words | 等待 RX，5ms 检查超时，遥测50–1000ms | UART1，RX4×200，协议与分发 |
| IMU | Low | 256 words | 10ms，启动零漂采样 | 六轴测量元组、短临界区快照 |
| LCD | Idle | 512 words | 50ms | 显示与页面状态 |
| Key/RGB | Idle | 各128 words | 原低速周期 | 按键/RGB |
| Log | Idle | 256 words | 队列事件，批量64字节，TX≤20ms | UART3，队列256字节，满则丢日志 |

配置栈为 FreeRTOS 的 32 位项数，不是实测余量。厂商 SDK/内核保持原版；GCC 使用匹配内核端口。具体版本见 SOURCES.md。
