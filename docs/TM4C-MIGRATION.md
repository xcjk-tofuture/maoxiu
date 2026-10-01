# TM4C 移植状态：尚未完成验收

分支 board/tm4c 从 maoxiu 公共协议与底盘服务基线建立。早期 xing-shu-ECB-TM4C 原仓库未改，移植来源 SHA 在 baseline 记录。沿用其 TM4C1233H6PM、ARMCC5.06u7、TM4C_DFP1.1.0、CMSIS5.9.0、CMSIS-FreeRTOS10.5.1；不替换 RTOS。

原板只有 PWM 两组/QEI 两路，因此实现两轮差速适配。四轮业务没有伪造为相同硬件。基础协议与STM32一致，保留设备ID与能力差异。删掉上电固定 -4000/+6000 的演示任务，PWM由10ms控制任务持有；UART0只传协议，日志移到UART1。QEI1初始化修正，PWM零值下溢修正，UART0中断按FreeRTOS阈值设置。

待基础验收：确定实际板卡/电机接线、QEI方向、轮距、轮周长、编码器倍率、ADC电池比例、电机PI和IMU量程。因原源码缺少这些可验证值，当前编译配置禁止运动、遥测电压/IMU为未校准占位值0，能力查询不宣称运动可用。不能称为与STM32同等硬件验收完成。此项属于本次基础重构整改，不属于后续功能TODO。

目标工程 MDK-ARM/tm4c/maoxiu_tm4c.uvprojx。需原版本Keil组件包；本机未找到ARMCC和目标板，构建、Flash/RAM、栈与硬件时序未验证。仅公共服务主机测试可执行。无后续功能TODO，不自动同步STM32的dev功能。
