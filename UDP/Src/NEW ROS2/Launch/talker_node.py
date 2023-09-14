from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, LogInfo
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'enable_talker',
            default_value='true',
            description='Set to true to launch the talker_node'
        ),
        Node(
            package='my_first_awesome_pkg',  # Replace with the actual package name
            executable='talker_node',       # Replace with the name of the executable
            name='talker_node',
            output='screen',
            condition=IfCondition(LaunchConfiguration('enable_talker'))
        ),
        # Add more nodes here as needed.
    ])
