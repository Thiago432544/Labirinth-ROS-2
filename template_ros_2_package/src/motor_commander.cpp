#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

// eu sei q nem precisava desse node, mas pra ficar mais completo e organizado, deixei aqui

class MotorCommander : public rclcpp::Node {
  
private:
  // recebe os comando da central e manda pros motores
  void wheel_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg){

    double left = msg->data[0];
    double right = msg->data[1];

    std_msgs::msg::Float64MultiArray cmd_left;
    std_msgs::msg::Float64MultiArray cmd_right;

    cmd_left.data.clear();
    cmd_right.data.clear();

    cmd_left.data.push_back(left);
    cmd_right.data.push_back(right);

    motor1_pub_->publish(cmd_left);
    motor2_pub_->publish(cmd_right);
  }

  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor1_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor2_pub_;
  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr wheel_sub_;

public:
  MotorCommander() : Node("motor_commander"){
    //publishers dos motor
    motor1_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor1/commands", 10);
    motor2_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor2/commands", 10);

    // subscribers para feedback da central
    wheel_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
      "/robot/wheel_speeds", 10,
      std::bind(&MotorCommander::wheel_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Motores turbinados acesos!");
  }
};
int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MotorCommander>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}