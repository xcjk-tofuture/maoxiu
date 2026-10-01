# 生成代码与手写分层的边界

CubeMX 原目录 `Core/`、`Drivers/`、`Middlewares/` 和 `.ioc` 保留；它们不按手写七层移动。厂商 SDK 与 RTOS 内核保持原版。手写七层统一位于 firmware/ 下：app/services/algorithms/drivers/platform/boards/os；避免 Windows 将 Drivers 与 drivers 视为同一目录。

`Core/Src/freertos.c` 保留原生成模板，所有启动桥接、任务重复定义屏蔽都在 USER CODE 区；实际任务编排在 `firmware/app/startup.c`。CubeMX 开启 Keep User Code 后，修改 IO 并重新生成应保留这些桥接；首次重新生成仍需检查差异并完整编译。CubeMX 内的旧任务列表仅作生成模板，实际任务的周期、优先级和栈以 app 编排为准。

现有 GCC 迁移要求继续执行：CMake 引用原生成目录，生成文件和手写文件分开列出。GCC RTOS 平台端口作为同版本补充独立管理，不能由 CubeMX 重新生成覆盖。Debug/Release GCC 编译已通过，旧 Keil 工程从当前源码移除，可从原 Git 提交恢复。AR 保留海思构建环境。

重新生成验证待办（重构验收项）：保存当前提交，在同版本 STM32Cube FW 包下开启 Keep User Code、只修改指定 IO；检查 USER CODE 桥接、DMA 回调、FreeRTOSConfig 堆配置和 HAL 时基仍正确，CMake 完整构建，再上板回归。当前未运行 CubeMX GUI，因此不声称已实测重新生成。
