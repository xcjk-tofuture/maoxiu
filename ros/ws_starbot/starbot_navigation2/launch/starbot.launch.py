'''
作者: 小鱼
公众号: 鱼香ROS
QQ交流群: 2642868461
描述: Nav2 launch启动文件
'''
import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (DeclareLaunchArgument, GroupAction,
                            IncludeLaunchDescription, SetEnvironmentVariable)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    #=============================1.定位到包的地址=============================================================
    fishbot_navigation2_dir = get_package_share_directory('starbot_navigation2')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')
    starbot_robot_dir = get_package_share_directory('starbot_serial')
    starbot_launch_dir = os.path.join(starbot_robot_dir, 'launch')
    stdout_linebuf_envvar = SetEnvironmentVariable(
        'RCUTILS_LOGGING_BUFFERED_STREAM', '1')
    
    #=============================2.声明参数，获取配置文件路径===================================================
    # use_sim_time 这里要设置成true,因为gazebo是仿真环境，其时间是通过/clock话题获取，而不是系统时间
    use_sim_time = LaunchConfiguration('use_sim_time', default='false') 
    map_yaml_path = LaunchConfiguration('map',default=os.path.join(fishbot_navigation2_dir,'maps','starbot_1_map.yaml
                                                                   '))
    nav2_param_path = LaunchConfiguration('params_file',default=os.path.join(fishbot_navigation2_dir,'param','starbot_nav2.yaml'))
    rviz_config_dir = os.path.join(nav2_bringup_dir,'rviz','nav2_default_view.rviz')

    #=============================3.声明启动launch文件，传入：地图路径、是否使用仿真时间以及nav2参数文件==============
    
    starbot_lidar = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(starbot_launch_dir, 'wheeltec_lidar.launch.py')),
    )

    starbot_robot = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(starbot_launch_dir, 'starbot_car.launch.py')),
    )

    
    nav2_bringup_launch = IncludeLaunchDescription(
            PythonLaunchDescriptionSource([nav2_bringup_dir,'/launch','/bringup_launch.py']),
            launch_arguments={
                'map': map_yaml_path,
                'use_sim_time': use_sim_time,
                'params_file': nav2_param_path}.items(),
        )
    rviz_node =  Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            parameters=[{'use_sim_time': use_sim_time}],
            output='screen')
    
    return LaunchDescription([starbot_lidar,starbot_robot, stdout_linebuf_envvar,nav2_bringup_launch,rviz_node])