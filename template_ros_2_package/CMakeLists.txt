cmake_minimum_required(VERSION 3.8)
project(template_ros_2_package)

if(POLICY CMP0148)
  cmake_policy(SET CMP0148 OLD)
endif()

# C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependências
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(include)

add_executable(motor_commander src/motor_commander.cpp)
ament_target_dependencies(motor_commander rclcpp std_msgs)
add_executable(sensors_node src/sensors.cpp)
ament_target_dependencies(sensors_node rclcpp sensor_msgs)
install(TARGETS
  motor_commander
  sensors_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_target_dependencies(
  motor_commander
  rclcpp
  std_msgs
  sensor_msgs
)

ament_package()
