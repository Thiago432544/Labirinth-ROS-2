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
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(geometry_msgs REQUIRED)

include_directories(include)

add_executable(sensors src/sensors.cpp)
ament_target_dependencies(sensors
  rclcpp
  std_msgs
  sensor_msgs
)

add_executable(motor_commander src/motor_commander.cpp)
ament_target_dependencies(motor_commander
  rclcpp
  std_msgs        
  sensor_msgs 
)

add_executable(central src/central.cpp)
ament_target_dependencies(central
  rclcpp
  std_msgs
  sensor_msgs
)
add_executable(camera src/camera.cpp)
ament_target_dependencies(camera
rclcpp
sensor_msgs
cv_bridge
geometry_msgs
)
target_link_libraries(camera
  ${OpenCV_LIBRARIES}
)


install(TARGETS
  sensors
  motor_commander
  central
  camera
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)


ament_package()
