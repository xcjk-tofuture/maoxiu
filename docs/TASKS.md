# 任务与资源所有权

| 任务 | 优先级 | 配置栈 | 周期/等待 | 资源与失败策略 |
|---|---|---|---|---|
| Control | 3 | 512 words | 10ms固定周期 | 命令队列4，QEI/ADC/双轮PWM；未标定状态4 |
| PC | 2 | 768 words | 字节事件/5ms超时检查 | UART0，256字节队列；ISR最多16字节，TX≤20ms |
| LCD | 1 | 512 words | 100ms | 电压/姿态快照与显示 |
| IMU | 1 | 256 words | 原10ms读取 | I²C测量与姿态快照 |
| RGB/Key | 1 | 128 words | 按键队列/1000ms心跳 | GPIOF ISR只投递事件，队列4；单写入者更新LED |
| Buzzer | 1 | 128 words | 原启动音乐/低电压告警 | 读取电压快照，保留原行为 |

STM32 CMSIS-RTOS v1 适配器直接把 stacksize 传给 FreeRTOS xTaskCreate，因此这里是32位 words；AR CMSIS-RTOS2 是 bytes。配置值不是实测高水位。STM32堆24KiB，TM4C堆16KiB；MSP与newlib堆另由链接脚本预留。具体余量须测 uxTaskGetStackHighWaterMark 和空闲堆。
