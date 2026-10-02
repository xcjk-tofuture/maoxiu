# StarPleiades（昴宿）构建与验证

依赖 ARM GNU 13.3.Rel1、CMake ≥3.21、Ninja 和 OpenOCD；VS Code 使用 C/C++、CMake Tools、Cortex-Debug。`ARM_GCC_PATH` 指向工具链根目录，相关 bin 加入 PATH。

```sh
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
python tests/run_host.py --cc gcc
python tests/run_pid.py --cc gcc
python tests/run_math.py --cc gcc
```

固件 `maoxiu_stm32.elf/.hex/.bin/.map` 和 `.su` 位于 `build/debug` 或 `build/release`。下载使用 ST-Link/SWD：连接匹配目标后执行 `cmake --build build/debug --target flash`。分区见 `firmware/boards/stm32/firmware.ld`。

Debug/Release 构建和主机算法/协议已有软件验证；实际 IO、量程、方向、闭环时序、栈/队列、故障停止和 CubeMX 重生成仍需实物回归。ROS2 完整联调针对 StarPleiades 单独验证。软件通过不替代硬件验收。
协议 v1 不兼容旧客户端，使用配套客户端和固件；历史版本从 Git 历史在独立 checkout 查阅。功能开发和扩展说明见 `dev`。
