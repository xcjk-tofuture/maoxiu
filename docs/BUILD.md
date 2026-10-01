ARM GNU 13.3.Rel1 / gcc 13.3.1；CMake 4.x（实际版本见 validation）；Ninja 1.13.2；xPack OpenOCD 0.12.0-7。源码没有迁移 SDK 或已有 RTOS 版本。Windows 工具放在桌面 embedded-tools，ARM_GCC_PATH 指向工具链根目录，PATH 包含其 bin、OpenOCD bin 与 Ninja。Linux/macOS 安装相应工具并添加 PATH，CMake 不含本机绝对路径。VS Code 扩展为 C/C++、CMake Tools、Cortex-Debug。

```sh
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
# 连接目标板后才执行实际烧录：
cmake --build build/debug --target flash
```

生成 ELF/HEX/BIN/MAP 和 GCC .su 文件位于 build/debug 或 build/release。Ctrl+Shift+B 会先 configure 再 build，F5 启动 OpenOCD 调试。STM32 使用 ST-Link / SWD，TM4C 使用板载 ICDI / SWD；驱动和 USB 权限须在本机配置。已经验证配置解析及命令生成，尚未连接探针执行 program/verify/reset。openocd --version 的具体开发版字符串与发行包号可能不同。

每次发布把固件、MAP、校验值和测试报告放入同一版本目录，注明源码提交、板卡及构建类型。当前二进制只供台架验证，实际引脚、电机正反转、量程与时序须先验收。不要把本机 CMakeCache 或 build 目录提交到 Git。
