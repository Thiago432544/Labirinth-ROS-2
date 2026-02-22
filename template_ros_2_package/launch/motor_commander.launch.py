from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='template_ros_2_package',
            executable='motor_commander',
            name='motor_commander',
            output='screen'
        )
    ])

