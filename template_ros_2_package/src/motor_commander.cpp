#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class MotorCommander : public rclcpp::Node
{
public:
  MotorCommander() : Node("motor_commander"){
    //declarando publisher
    motor1_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor1/commands", 10);
    motor2_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor2/commands", 10);

    timer_ = this->create_wall_timer(
      100ms,  
      std::bind(&MotorCommander::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "ACELEEERAAAAAAAAAAAA");
  }

private:
  void timer_callback(){
    std_msgs::msg::Float64MultiArray cmd_left;
    std_msgs::msg::Float64MultiArray cmd_right;

    float right = 0.1;
    float left = 0.1;

    cmd_left.data.push_back(left);
    cmd_right.data.push_back(right);

    motor1_pub_->publish(cmd_left);
    motor2_pub_->publish(cmd_right);

  }

  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor1_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor2_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MotorCommander>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
