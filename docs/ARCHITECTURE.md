# StarPleiades STM32 架构

## 启动入口

`firmware/platform/stm32/startup_stm32f407xx.S: Reset_Handler` 初始化数据段后进入 `Core/Src/main.c: main`。
`main` 初始化 HAL、时钟、外设及 DMA，再调用 `MX_FREERTOS_Init` 和 `osKernelStart`。
`Core/Src/freertos.c` 的 USER CODE 桥接到 `firmware/app/startup.c: app_tasks_init`。
`APP_RTOS_EXTERNAL_TASKS=1` 排除生成模板里的旧任务；实际任务表以 `startup.c` 为准。

## 目录与生成代码

`Core/`、`Drivers/`、`Middlewares/`、`.ioc` 保留 CubeMX 生成结构。手写代码位于 `firmware/`：

| 目录 | 职责 |
|---|---|
| `app` | 启动、任务、业务流程及状态机 |
| `services` | 控制、采集、协议、显示和参数管理 |
| `algorithms` | 独立 PID、滤波、运动学和姿态计算 |
| `drivers` | 电机、传感器、显示和存储设备 |
| `platform` | UART、SPI、PWM、时间等芯片实现 |
| `boards` | 引脚资源、板卡参数及链接脚本 |
| `os` | 消息、快照、互斥、故障钩子及同版本 GCC 移植适配 |

依赖方向：应用 → 服务 → 设备 → 平台 → SDK；服务调用独立算法，任务及共享资源使用必要的 RTOS 适配。`services/legacy` 保留旧流程与适配接口，目录名不代表所有旧模块已彻底拆分。

CubeMX 调整外设后开启 Keep User Code，核查 USER CODE 桥接、DMA 回调、FreeRTOS 配置、HAL 时基和 `cmake/sources.cmake`，再完整构建。厂商源码及 RTOS 内核不随手写层重排。

## 底盘数据流

UART1 DMA/IDLE → `PC_Data_Rx_Proc` → RX 队列 → `PC_Task_Proc` → 帧解析/分发 → `chassis_port_submit` → 命令队列 → `Encoder_Task_Proc` → 运动学/PI → 电机接口 → TIM/PWM。

`Encoder_Task_Proc` 是完整闭环任务：10 ms 读取编码器和电压、消费命令与配置、调用 `chassis_step`，正常输出保持 20 ms 节拍，非 active 状态当前控制步输出零。闭环 `service` 仅由该任务更新；遥测和显示复制受保护快照。

| 模块 | 源码 |
|---|---|
| 闭环与参数应用 | `firmware/app/tasks/chassis_task.c` |
| 状态判断 | `firmware/services/chassis_service.c` |
| 麦轮/差速正逆解、增量 PI | `firmware/algorithms/chassis/chassis_math.c` |
| 电机设备 / 平台 | `firmware/drivers/motor/stm32_motor.c` / `firmware/platform/stm32/motor_hal.c` |
| 资源与默认参数 | `firmware/boards/stm32/motor_resources.h` / `maoxiu_board.h` |
| 参数范围与存储记录 | `firmware/services/parameters/` |

状态：0 idle、1 active、2 欠压、3 命令超时、4 未标定（供 TM4C）。默认命令超时 500 ms，欠压阈值 6 V。编译默认参数可能被启动时加载的有效记录覆盖。

## 任务与所有权

| 任务入口 | 优先级 | 栈（32 位项） | 周期 / 资源 |
|---|---|---|---|
| `Encoder_Task_Proc` | High | 512 | 10 ms；命令队列 4、配置队列 1、编码器/ADC/PWM |
| `PC_Task_Proc` | Normal | 768 | RX 队列 4×200 字节，等待 5 ms；UART1 TX 唯一所有者 |
| `IMU_Task_Proc` | Low | 256 | 10 ms；MPU6050 测量及快照 |
| `LCD_Task_Proc` | Idle | 512 | 50 ms；显示 |
| Key / RGB | Idle | 各 128 | 低速交互 |
| `Ble_Uart3_Task_Proc` | Idle | 256 | 日志队列；UART3，满则丢日志 |

CMSIS v1 适配实际把栈配置按 FreeRTOS 项数传入；配置值不是实测栈水位。中断只做有限复制和 FromISR 投递；临界区只用于短快照操作，禁止阻塞 I/O。

参数更新只在停止反馈条件满足时入队，控制任务应用后复位 PI 并更新 revision。显式保存先进入停止维护模式、关闭 PWM，再写 Flash；退出维护时清除旧速度命令。具体接口见 [协议](PROTOCOL.md)。

## ROS 配套与故障路径

`ros/ws_starbot/starbot_serial/setup.py` 注册 `starbot_serial.starbot_serial:main`。节点订阅 `cmd_vel`，编码 v1 速度帧；5 ms 定时器解码状态，积分速度并发布 `odom`、`imu/data_raw`、`PowerVoltage`、`robotpose` 和 `robotvel`。

已接入的初始化失败、分配失败和栈溢出路径进入 `app/failure.c`，由平台关闭输出后停机。没有已验收的硬件看门狗自动恢复。构建和硬件检查见 [BUILD.md](BUILD.md)。
