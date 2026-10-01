# 回滚与 Git 流程

原始源码基线：`7bf9865`。`git switch --detach <commit>` 在独立副本检查旧固件；不要在带未提交改动的目录做 reset --hard。原 Keil 工程/机械臂演示及实验代码可从原提交恢复；当前主构建已使用 CMake/GCC/OpenOCD。TM4C 源平台参考早期 xing-shu-ECB-TM4C 原提交，参考仓库本身未修改。

BREAKING CHANGE：上位机统一改为协议 v1，旧客户端与新固件不能混用；ROS配套串口节点同步调整。星璇修正了 PID/滤波时间单位、滤波历史及错误传播，实际增益和闭环响应必须重新台架验收。候选功能在 dev，验收后合回 master；TM4C仅选择性同步适用基础改动。提交用 类型(模块): 描述，格式提交独立于逻辑提交。当前未执行 push。
