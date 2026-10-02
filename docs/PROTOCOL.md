# StarPleiades TM4C 串口协议

## 帧格式

`A5 5A | version:u8 | flags:u8 | sequence:u16 | command:u16 | length:u16 | payload | CRC16:u16`

版本 1；所有多字节字段小端。payload 最大 128 字节，接收超时 100 ms。CRC16/CCITT-FALSE：poly=0x1021、init=0xFFFF，覆盖 version 到 payload。
flags：0 请求、1 应答、2 主动遥测。应答/状态事件 payload 首字节为状态码：0 成功、1 长度错误、2 不支持、3 范围错误、4 状态拒绝、5 忙、6 内部错误。

基础命令：1 版本、2 设备 ID、3 能力、4 状态、5 参数读、6 参数写。参数 ID 1 为遥测周期：读取 payload 为 `id:u16`，写入为 `id:u16 + value:u16`。遥测周期 50..1000 ms。

编解码与分发在 `services/protocol`，不直接调用硬件或修改控制状态。中断交接数据，通信任务拥有解析器和发送；项目 `business` 回调通过业务接口提交或查询。分包、粘包、异常帧与队列丢包恢复由解析/传输层处理。旧客户端不兼容 v1，需配套固件和客户端。

## 板载接口

UART0，115200。设备 ID `0x4d540001`，能力为状态与遥测周期。状态共 31 字节，与 STM32 底盘布局一致：状态码、底盘状态、vx/vy/wz、轮数、电压及六个 IMU i16 字段。
轮数为 2；IMU 字段为 0；未标定状态为 4。电压使用原 ADC 比例，需实物校验。

`0x1000` 入参是 vx/vy/wz 三个 LE f32（m/s、m/s、rad/s）；差速板要求 vy=0，vx/wz 限 ±2/±6。未标定返回状态拒绝，不开放运动能力。
不提供 STM32 的参数保存、底盘诊断或后续功能扩展。
