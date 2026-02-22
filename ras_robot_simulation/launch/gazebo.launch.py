import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, ExecuteProcess, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution, Command
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.event_handlers import OnProcessExit

def generate_launch_description():
    pkg_share = FindPackageShare(package='robot_ras_ros').find('robot_ras_ros')
    worlds_dir = os.path.join(pkg_share, 'worlds')
    default_world = 'labirinth.world'
    
    world_gazebo_arg = DeclareLaunchArgument(
        name="world", 
        default_value=default_world, 
        description="Name of the Gazebo world file located in the worlds directory"
    )
    
    world_gazebo_path = PathJoinSubstitution([worlds_dir, LaunchConfiguration("world")])
    
    world_models_path = os.path.join(pkg_share, 'models')
    install_dir = FindPackageShare(package='robot_ras_ros').find('robot_ras_ros')

    os.environ['GAZEBO_MODEL_PATH'] = world_models_path
    os.environ['GAZEBO_PLUGIN_PATH'] = os.environ.get('GAZEBO_PLUGIN_PATH', '') + ':' + install_dir + '/lib'

    print("GAZEBO MODEL PATH==", os.environ["GAZEBO_MODEL_PATH"])
    print("GAZEBO PLUGIN PATH==", os.environ["GAZEBO_PLUGIN_PATH"])

    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            os.path.join(FindPackageShare("gazebo_ros").find("gazebo_ros"), "launch", "gazebo.launch.py")
        ]),
        launch_arguments={"world": world_gazebo_path}.items(),
    )

   
    use_sim_time = LaunchConfiguration('use_sim_time', default='true')

    xacro_file_name = 'robot.urdf.xacro'

    # Caminho para o arquivo URDF/XACRO
    xacro_file = os.path.join(pkg_share, 'urdf', xacro_file_name)
    robot_description_config = Command(['xacro ', xacro_file, ' use_sim_time:=', use_sim_time])

    # Robot State Publisher
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        # namespace='robot_ras',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time,
                     'robot_description': robot_description_config}]
    )

    # Spawn do Robô no Gazebo
    spawn_entity_node = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-topic', 'robot_description',
                   '-entity', 'ras_robot', # Nome da entidade no Gazebo
                   '-x', '0.0', # Posição inicial do robô (será sobrescrita pela pose no chassis.xacro se definida lá)
                   '-y', '0.0',
                   '-z', '0.0', # Ajuste conforme necessário para que o robô não caia pelo chão antes da física estabilizar
                   '-R', '0.0', # Ajuste conforme necessário para que o robô não caia pelo chão antes da física estabilizar
                   '-P', '0.0', # Ajuste conforme necessário para que o robô não caia pelo chão antes da física estabilizar
                   '-Y', '0.0'],
        output='screen'
    )

    # Carregar Controladores (JointStateBroadcaster e os de velocidade)
    load_joint_state_broadcaster = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active',
             'joint_state_broadcaster'],
        output='screen'
    )

    load_left_wheel_controller = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active', 'motor1'],
        output='screen'
    )
    load_right_wheel_controller = ExecuteProcess(
        cmd=['ros2', 'control', 'load_controller', '--set-state', 'active', 'motor2'],
        output='screen'
    )

    return LaunchDescription([
        world_gazebo_arg,
        gazebo_launch,
        DeclareLaunchArgument(
            'use_sim_time',
            default_value='true',
            description='Use simulation (Gazebo) clock if true'),

        robot_state_publisher_node,
        spawn_entity_node,

        RegisterEventHandler(
            event_handler=OnProcessExit(
                target_action=spawn_entity_node,
                on_exit=[
                    load_joint_state_broadcaster,
                    load_left_wheel_controller,
                    load_right_wheel_controller,
                ],
            )
        ),
    ])
