# 任务与资源所有权

| 任务 | 优先级 | 配置栈 | 周期/等待 | 资源与失败策略 |
|---|---|---|---|---|
| Control | High | 512 words | 10ms 固定周期，输出 20ms；故障立即停止 | 闭环状态、命令队列4、ADC/编码器/PWM |
| PC | Normal | 768 words | 等待 RX，5ms 检查超时，遥测50–1000ms | UART1，RX4×200，协议与分发 |
| IMU | Low | 256 words | 10ms，启动零漂采样 | 六轴测量元组、短临界区快照 |
| LCD | Idle | 512 words | 50ms | 显示与页面状态 |
| Key/RGB | Idle | 各128 words | 原低速周期 | 按键/RGB |
| Log | Idle | 256 words | 队列事件，批量64字节，TX≤20ms | UART3，队列256字节，满则丢日志 |

STM32 CMSIS-RTOS v1 适配器直接把 stacksize 传给 FreeRTOS xTaskCreate，因此这里是32位 words；AR CMSIS-RTOS2 是 bytes。配置值不是实测高水位。STM32堆24KiB，TM4C堆16KiB；MSP与newlib堆另由链接脚本预留。具体余量须测 uxTaskGetStackHighWaterMark 和空闲堆。
