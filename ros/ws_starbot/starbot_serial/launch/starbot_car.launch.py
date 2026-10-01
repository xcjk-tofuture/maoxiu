import os
from pathlib import Path
from launch import LaunchDescription
from ament_index_python.packages import get_package_share_directory
from launch.actions import (DeclareLaunchArgument, GroupAction,
                            IncludeLaunchDescription, SetEnvironmentVariable)
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, PythonExpression
def generate_launch_description():

    bringup_dir = get_package_share_directory('starbot_serial')
    launch_dir = os.path.join(bringup_dir, 'launch')

    carto_slam = LaunchConfiguration('carto_slam', default='false')
    carto_slam_dec = DeclareLaunchArgument('carto_slam',default_value='false')
             
    imu_config = Path(get_package_share_directory('starbot_serial'), 'config', 'imu.yaml')

    starbot_serial = Node(
        package='starbot_serial',
        executable='starbot_serial',
    )

        #choose your car,the default car is mini_mec 
    choose_car = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(launch_dir, 'robot_mode_description.launch.py')),
    )

    robot_ekf = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(os.path.join(launch_dir, 'starbot_ekf.launch.py')),
            launch_arguments={'carto_slam':carto_slam}.items(),            
    )

    base_to_link = Node(
            package='tf2_ros', 
            executable='static_transform_publisher', 
            name='base_to_link',
            arguments=['0', '0', '0','0', '0','0','base_footprint','base_link'],
    )
    base_to_gyro = Node(
            package='tf2_ros', 
            executable='static_transform_publisher', 
            name='base_to_gyro',
            arguments=['0.06', '0', '0','0', '0','0','base_footprint','gyro_link'],
    )

    imu_filter_node =  Node(
        package='imu_filter_madgwick',
        executable='imu_filter_madgwick_node',
        parameters=[imu_config]
    )

    joint_state_publisher_node = Node(
            package='joint_state_publisher', 
            executable='joint_state_publisher', 
            name='joint_state_publisher',
    )

    launch_description = LaunchDescription()
    launch_description.add_action(choose_car)
    launch_description.add_action(carto_slam_dec)
    launch_description.add_action(starbot_serial)
     
    launch_description.add_action(base_to_link)
    launch_description.add_action(base_to_gyro)
    launch_description.add_action(joint_state_publisher_node)

    launch_description.add_action(imu_filter_node)
    launch_description.add_action(robot_ekf)

    return launch_description


