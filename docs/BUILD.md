# maoxiu TM4C构建与验证

依赖：ARM GNU 13.3.Rel1、CMake ≥3.21、Ninja、OpenOCD；VS Code 扩展 C/C++、CMake Tools、Cortex-Debug。`ARM_GCC_PATH` 可指向工具链根目录，相关 bin 目录加入 PATH。

```sh
cmake --preset debug
cmake --build --preset debug
cmake --preset release
cmake --build --preset release
```

输出为 `build/debug` 或 `build/release` 下的 `maoxiu_tm4c.elf/.hex/.bin/.map` 和 GCC `.su`。VS Code `Ctrl+Shift+B` 配置后构建；F5 使用 OpenOCD 调试。调试接口：板载 ICDI / SWD。

连接匹配的板卡与调试器后，可运行：

```sh
cmake --build build/debug --target flash
```

下载配置使用 `program ... verify reset exit`。ELF 由启动汇编复制 `.data`、清零 `.bss`；堆、栈和 `.bss` 不作为烧录数据。Flash/RAM 分区见链接脚本，`.su` 不替代 RTOS 实测栈余量。

## 软件检查

```sh
python tests/run_host.py --cc gcc
```

公共协议和底盘算法可在主机测试；实际 RTOS/总线行为需台架验证。

## 硬件验证

实测板卡接线、QEI 方向、轮距/轮周长/编码器比例、电池 ADC、PI 及 IMU 量程；确认周期抖动、栈水位、IRQ 优先级、队列满和创建失败行为。标定前保持禁止运动。

Debug/Release 软件构建及主机测试已有通过记录；探针烧录、实时任务水位和硬件行为仍待实际板卡验证。发布固件、map、报告应关联源码版本保存，构建目录不提交。

## 版本兼容与回滚

旧源码提交 `7bf9865` 可在独立 checkout 中检查；协议/参数格式按对应固件版本使用。功能在 dev 完成验收后合入 master，TM4C 仅同步适用的基础变更。不要以旧参数格式解释新版记录。
