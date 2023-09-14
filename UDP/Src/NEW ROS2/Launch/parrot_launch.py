from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, LogInfo
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'enable_parrot',
            default_value='true',
            description='Set to true to launch the parrot node'
        ),
        Node(
            package='your_package_name',  # Replace with the actual package name
            executable='parrot',         # Replace with the name of the executable
            name='parrot',
            output='screen',
            condition=IfCondition(LaunchConfiguration('enable_parrot'))
        ),
        # Add more nodes here as needed.
    ])