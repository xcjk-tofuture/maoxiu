# TM4C SDK 边界

从参考仓库导入的 TivaWare 保留 `Lib/inc`、`Lib/src`、`utils` 原目录与内容。厂商文件不做业务重构。手写适配放 platform/tm4c，设备实现放 drivers/tm4c，板卡参数放 boards/tm4c；引用 SDK 时由构建系统声明 include 与 source。RTOS 内核版本仍固定为原 10.5.1，GCC 端口单独补齐。SDK 升级另列任务。
