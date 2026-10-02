# 依赖与来源

原文件版权、许可证和版本注释保留；第三方版本不能确认时不补造版本或许可证。

- STM32Cube FW_F4 V1.27.1；FreeRTOS 10.3.1 原内核。
- GCC CM4F 端口：[匹配内核的官方源码](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V10.3.1-kernel-only/portable/GCC/ARM_CM4F)，位于 `firmware/os/ports`。
- GNU 启动汇编：[ST CMSIS Device F4 v2.6.8](https://github.com/STMicroelectronics/cmsis-device-f4/tree/v2.6.8/Source/Templates/gcc)，未升级原 HAL。
- ARM GNU 13.3.Rel1：Windows 包 SHA256 `e46fda043c0ce83582bc8db4b3ef85f77f4beb7333344c2f4193c17e1167a095`；CI Linux 包 SHA256 `95c011cee430e64dd6087c75c800f04b9c49832cc1000127a92a97f9c8d83af4`。
- xPack OpenOCD 0.12.0-7：Windows 包 SHA256 `6bfd3c97135aafef8affc9af1acf34fd0e2b9ca26044506f6abd7f95b7630052`。
- CI 依赖与固定提交见 `.github/workflows/firmware.yml`。

ROS 工作区包含 Cartographer/Nav2/RealSense/雷达等导入源码，保留各自版权；缺少完整上游 Git 信息的组件版本不能唯一确认。
