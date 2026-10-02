# StarPleiades TM4C 架构

## 入口与目录

`platform/tm4c/startup_tm4c123.S` → `app/tm4c/main.c: main` → `All_Init` → `FREERTOS_Init` → `vTaskStartScheduler`。
应用、服务、算法、设备、平台、板级和 RTOS 适配分别位于根目录的 `app/services/algorithms/drivers/platform/boards/os`；厂商 `Lib/`、`utils/` 保留原结构与版权。

## 数据流与资源

UART0 ISR → 256 字节队列 → `app/tm4c/pc_service.c` 帧解析/分发 → 命令队列 4 → `app/tm4c/control_task.c` → 公共 `chassis_service/chassis_math` → `drivers/tm4c/maoxiu_motor.c` → `platform/tm4c` QEI/PWM。
控制任务拥有闭环状态，显示/通信读取快照。当前板型为两轮差速；`vy` 必须为 0。

| 任务 | 优先级 | 栈（32 位项） | 资源 |
|---|---|---|---|
| Control | 3 | 512 | 10 ms，QEI/ADC/双轮 PWM |
| PC | 2 | 768 | UART0 字节队列，5 ms 超时检查，TX 有界 |
| LCD | 1 | 512 | 显示快照 |
| IMU | 1 | 256 | 保留 MPU6050 采集与快照 |
| RGB / 按键事件 | 1 | 128 | GPIOF 事件、指示灯 |
| Buzzer | 1 | 128 | 启动音乐、电压告警 |

实际任务定义见 `app/tm4c/FreeRTOS.c`。创建失败进入 `tm4c_fatal`，关闭输出后停机。

## 未标定状态

`boards/tm4c/maoxiu_board.h` 中 `MAOXIU_TM4C_CALIBRATED=0`：拒绝速度命令、发布状态 4、保持零 PWM，不声明运动能力。轮距、每计数距离、PI 和电池比例需实物标定；IMU 未接入统一状态包，字段填零。

公共业务、算法及基础帧规则与 STM32 对齐，芯片适配和板型差异独立；当前是源码副本，需要核对公共修复。该分支只维护基础构建、标定与硬件基线，不安排后续功能开发。
