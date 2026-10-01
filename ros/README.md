# maoxiu ROS2 配套工作区

从用户指定的桌面 `ws_starbot` 复制；原目录未修改。包路径保持原样，便于回滚现有 launch。`import-manifest.json` 固定导入内容的 SHA256；第三方源码的精确上游提交尚未提供，不能把目录内容当作最新发行版。

自有包：starbot_serial、starbot_ros2_interfaces、starbot_keyboard、starbot_urdf、starbot_cartographer、starbot_navigation2。第三方目录见导入清单，各自 LICENSE、package.xml 和来源说明随源码保留。上游 TODO 与项目 TODO 分开记录；未改厂商导航、SLAM、相机和雷达库。

在原 ROS2 Linux 环境运行 `rosdep install --from-paths . --ignore-src -r -y`，再 `colcon build`，`source install/setup.bash`。本机 Windows 尚无 ROS2/colcon，不宣称 ROS launch 或硬件回归已通过。单独协议检查：`python -m unittest discover -s starbot_serial/test -p test_star_protocol.py`。

启动串口节点：`ros2 run starbot_serial starbot_serial --ros-args -p port:=/dev/ttyACM0 -p baudrate:=115200`。节点默认协议v1，必须配套本次固件。收到 cmd_vel 后发送三个 SI float32；固件欠压拒绝、500ms命令超时停机。持续行驶需持续发布 cmd_vel。网络功能、导航业务本次不新增。
