from geometry_msgs.msg import PoseStamped
from nav2_simple_commander.robot_navigator import BasicNavigator,TaskResult
import rclpy
from rclpy.duration import Duration
from copy import deepcopy


rclpy.init()
navigator = BasicNavigator() 
navigator.waitUntilNav2Active()

# ======================初始化位置，代替rviz2的2D Pose Estimate===============================
initial_pose = PoseStamped()
initial_pose.header.frame_id = 'map'
initial_pose.header.stamp = navigator.get_clock().now().to_msg()
initial_pose.pose.position.x = 0.0
initial_pose.pose.position.y = 0.0
initial_pose.pose.orientation.w = 1.0
navigator.setInitialPose(initial_pose)

#========================导航到目标点1===========================================
nav_start = navigator.get_clock().now()

goal_pose1 = deepcopy(initial_pose)
goal_pose1.pose.position.x = 0.5
goal_pose1.pose.position.y = 0.5
navigator.goToPose(goal_pose1)
while not navigator.isTaskComplete():
  feedback = navigator.getFeedback()
  now = navigator.get_clock().now()
  #检查是否超时，超时则停止导航到点   
  if now - nav_start > Duration(seconds=600):
    navigator.cancelTask()


#================================导航到目标点2==================================
nav_start = navigator.get_clock().now()

goal_pose2 = deepcopy(initial_pose)
goal_pose2.pose.position.x = -0.5
oal_pose2.pose.position.y = 0.5
navigator.goToPose(goal_pose2)
while not navigator.isTaskComplete():
  feedback = navigator.getFeedback()
  now = navigator.get_clock().now()
  #检查是否超时，超时则停止导航到点   
  if now - nav_start  > Duration(seconds=600):
    navigator.cancelTask()


#================================导航到原点==================================
nav_start = navigator.get_clock().now()

goal_pose3 = deepcopy(initial_pose)
goal_pose3.pose.position.x = 0.0
navigator.goToPose(goal_pose3)
while not navigator.isTaskComplete():
  feedback = navigator.getFeedback()
  now = navigator.get_clock().now()
  #检查是否超时，超时则停止导航到点   
  if now - nav_start  > Duration(seconds=600):
    navigator.cancelTask()

#===============================查看返回结果=====================================
result = navigator.getResult()
if result == TaskResult.SUCCEEDED:
    print('Goal succeeded!')
elif result == TaskResult.CANCELED:
    print('Goal was canceled!')
elif result == TaskResult.FAILED:
    print('Goal failed!')