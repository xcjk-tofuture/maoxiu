# 源码、SDK 与工具来源

原始提交见 baseline/inventory.json；原文件版权、许可证和版本注释保留，未为来源不明的第三方代码补造许可证。

- maoxiu STM32 固定 STM32Cube FW_F4 V1.27.1，星璇固定 V1.26.2；各自 FreeRTOS 10.3.1 原源码保留。
- 匹配 GCC RTOS 端口：https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V10.3.1-kernel-only/portable/GCC/ARM_CM4F 。
- TM4C 新 RTOS：https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V10.5.1 ，原 BSD/MIT 注释与 LICENSE 保留；Lib/utils 与参考源码字节相同。
- STM32 GNU 启动汇编：https://github.com/STMicroelectronics/cmsis-device-f4/tree/v2.6.8/Source/Templates/gcc ，仅补充启动文件，未升级 HAL。
- TM4C 启动表根据原 ARM CMSIS V1.00 / 2013-05-15 文件翻译成 GNU 汇编，保留版权与分发说明。
- ARM GNU 13.3 官方包 SHA256：e46fda043c0ce83582bc8db4b3ef85f77f4beb7333344c2f4193c17e1167a095。
- xPack OpenOCD v0.12.0-7 官方发行包 SHA256：6bfd3c97135aafef8affc9af1acf34fd0e2b9ca26044506f6abd7f95b7630052。
- AR 主机测试 cJSON 1.7.18 来自 https://github.com/DaveGamble/cJSON/tree/v1.7.18 ，只用作测试依赖；AR 原 SDK 未升级。
- ROS ws_starbot 从用户桌面原目录复制，原目录未修改；Cartographer/Nav2/RealSense/雷达等第三方源码保留版权，其精确上游版本不能由无 Git 的本地副本完整确认，文件校验清单记录导入基线。

- Linux GCC13.3.Rel1包及SHA256来自[Arm官方发布文件](https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz.sha256asc)，值`95c011cee430e64dd6087c75c800f04b9c49832cc1000127a92a97f9c8d83af4`。
- CI [checkout](https://github.com/actions/checkout) v7提交`3d3c42e5aac5ba805825da76410c181273ba90b1`；[upload-artifact](https://github.com/actions/upload-artifact) v7提交`043fb46d1a93c77aae656e7c1c64a875d1fc6a0a`，通过官方Git标签核对。
