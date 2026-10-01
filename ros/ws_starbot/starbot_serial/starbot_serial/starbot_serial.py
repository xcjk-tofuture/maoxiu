"""ROS2 maoxiu serial service; one executor owns UART and published state."""
import math
import struct
import time
import serial
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist, Quaternion
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Imu
from std_msgs.msg import Float32
from starbot_ros2_interfaces.msg import Data
from .star_protocol import Frame, Parser, REQUEST, RESPONSE, EVENT, STATUS, CHASSIS_VELOCITY, decode_chassis_status
from .orientation import Orientation

class StarbotSerialNode(Node):
    def __init__(self):
        super().__init__('starbot_serial')
        self.declare_parameter('port', '/dev/ttyACM0')
        self.declare_parameter('baudrate', 115200)
        self.uart = serial.Serial(self.get_parameter('port').value,
                                  self.get_parameter('baudrate').value, timeout=0, write_timeout=0.05)
        self.parser = Parser()
        self.sequence = 0
        self.pending = {}
        self.fusion = Orientation()
        self.x = self.y = self.yaw = 0.0
        self.last_sample = None
        self.publishers = {
            'odom': self.create_publisher(Odometry, 'odom', 5),
            'imu': self.create_publisher(Imu, 'imu/data_raw', 5),
            'voltage': self.create_publisher(Float32, 'PowerVoltage', 1),
            'pose': self.create_publisher(Data, 'robotpose', 10),
            'velocity': self.create_publisher(Data, 'robotvel', 10),
        }
        self.create_subscription(Twist, 'cmd_vel', self.command, 1)
        self.create_timer(0.005, self.receive)

    def command(self, msg):
        values = msg.linear.x, msg.linear.y, msg.angular.z
        if not all(math.isfinite(v) for v in values) or any(abs(v) > limit for v, limit in zip(values, (2, 2, 6))):
            self.get_logger().error('cmd_vel outside firmware SI bounds')
            return
        sequence = self.sequence
        self.sequence = (sequence + 1) & 0xFFFF
        frame = Frame(REQUEST, sequence, CHASSIS_VELOCITY, struct.pack('<fff', *values)).encode()
        try:
            if self.uart.write(frame) != len(frame):
                self.get_logger().error('incomplete serial write')
                return
        except serial.SerialException as error:
            self.get_logger().error(str(error))
            return
        self.pending[sequence] = time.monotonic()

    def receive(self):
        now = time.monotonic()
        for sequence, started in list(self.pending.items()):
            if now - started > 0.5:
                del self.pending[sequence]
                self.get_logger().warning('velocity command acknowledgement timed out')
        try:
            data = self.uart.read(min(self.uart.in_waiting, 512))
        except serial.SerialException as error:
            self.get_logger().error(str(error))
            return
        for frame in self.parser.feed(data, int(now * 1000)):
            if frame.flags == RESPONSE and frame.command == CHASSIS_VELOCITY:
                if self.pending.pop(frame.sequence, None) is not None and frame.payload and frame.payload[0]:
                    self.get_logger().warning(f'firmware rejected velocity: status {frame.payload[0]}')
            elif frame.flags == EVENT and frame.command == STATUS:
                try:
                    state, vx, vy, wz, wheels, voltage, ax, ay, az, gx, gy, gz = decode_chassis_status(frame.payload)
                except ValueError:
                    continue
                if wheels not in (2, 4) or not all(math.isfinite(v) for v in (vx, vy, wz, voltage)):
                    continue
                dt = 0.0 if self.last_sample is None else now - self.last_sample
                self.last_sample = now
                if 0 < dt <= 0.5:
                    self.x += (vx * math.cos(self.yaw) - vy * math.sin(self.yaw)) * dt
                    self.y += (vx * math.sin(self.yaw) + vy * math.cos(self.yaw)) * dt
                    self.yaw += wz * dt
                self.publish(vx, vy, wz, voltage, (ax, ay, az), (gx, gy, gz), dt)

    def publish(self, vx, vy, wz, voltage, acceleration, gyroscope, dt):
        stamp = self.get_clock().now().to_msg()
        odom = Odometry()
        odom.header.stamp = stamp
        odom.header.frame_id = 'odom_combined'
        odom.child_frame_id = 'base_footprint'
        odom.pose.pose.position.x, odom.pose.pose.position.y = self.x, self.y
        odom.pose.pose.orientation = Quaternion(z=math.sin(self.yaw / 2), w=math.cos(self.yaw / 2))
        odom.twist.twist.linear.x, odom.twist.twist.linear.y = vx, vy
        odom.twist.twist.angular.z = wz
        self.publishers['odom'].publish(odom)
        imu = Imu()
        imu.header.stamp, imu.header.frame_id = stamp, 'gyro_link'
        acc = tuple(v / 1671.84 for v in acceleration)
        gyro = tuple(v * 0.00026644 for v in gyroscope)
        q = self.fusion.step(gyro, acc, dt)
        imu.orientation.w, imu.orientation.x, imu.orientation.y, imu.orientation.z = q
        imu.linear_acceleration.x, imu.linear_acceleration.y, imu.linear_acceleration.z = acc
        imu.angular_velocity.x, imu.angular_velocity.y, imu.angular_velocity.z = gyro
        imu.orientation_covariance[0] = imu.orientation_covariance[4] = 1e6
        imu.orientation_covariance[8] = 1e-6
        self.publishers['imu'].publish(imu)
        self.publishers['voltage'].publish(Float32(data=voltage))
        self.publishers['pose'].publish(Data(x=self.x, y=self.y, z=self.yaw))
        self.publishers['velocity'].publish(Data(x=vx, y=vy, z=wz))

    def destroy_node(self):
        self.uart.close()
        return super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = None
    try:
        node = StarbotSerialNode()
        rclpy.spin(node)
    finally:
        if node is not None:
            node.destroy_node()
        rclpy.shutdown()
