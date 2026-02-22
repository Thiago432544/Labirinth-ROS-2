#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"

using namespace std::chrono_literals;

class Sensors : public rclcpp::Node
{
public:
  Sensors() : Node("sensors"){
    // declaracao dos subscribers
    front_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/front", 10,
      std::bind(&Sensors::front_callback, this, std::placeholders::_1));

    rear_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/rear", 10,
      std::bind(&Sensors::rear_callback, this, std::placeholders::_1));

    left_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/left", 10,
      std::bind(&Sensors::left_callback, this, std::placeholders::_1));

    right_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/right", 10,
      std::bind(&Sensors::right_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Node de sensores iniciado, ouvindo /sensor_range/*");
  
  }

private:
  // atualizam os valores recebidos 
  void front_callback(const sensor_msgs::msg::Range::SharedPtr msg) {
    float real_dist = msg->range;
    RCLCPP_INFO_THROTTLE(this->get_logger(), 
    *this->get_clock(),
    1000,
    "Distância frontal = %.2f", real_dist);
}

  void rear_callback(const sensor_msgs::msg::Range::SharedPtr msg) {
    float real_dist = msg->range;
    RCLCPP_INFO_THROTTLE(this->get_logger(), 
    *this->get_clock(),
    1000,
    "Distância traseira: %.2f", real_dist);
}

  void left_callback(const sensor_msgs::msg::Range::SharedPtr msg) {
    float real_dist = msg->range;
    RCLCPP_INFO_THROTTLE(this->get_logger(), 
    *this->get_clock(),
    1000,
    "Distância esquerda: = %.2f", real_dist);
}

  void right_callback(const sensor_msgs::msg::Range::SharedPtr msg) {
    float real_dist = msg->range;
    RCLCPP_INFO_THROTTLE(this->get_logger(), 
    *this->get_clock(),
    1000,
    "Distância direita: %.2f", real_dist);
}
  // atribuindo o subscriber a cada respectivo sensor
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr front_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr rear_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr left_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr right_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Sensors>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}