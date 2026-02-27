#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class Sensors : public rclcpp::Node{

private:
  // armazena a ultima leitura
  void front_callback(const sensor_msgs::msg::Range::SharedPtr msg){
    dist_front_ = msg->range;
  }

  void rear_callback(const sensor_msgs::msg::Range::SharedPtr msg){
    dist_rear_ = msg->range;
  }

  void left_callback(const sensor_msgs::msg::Range::SharedPtr msg){
    dist_left_ = msg->range;
  }

  void right_callback(const sensor_msgs::msg::Range::SharedPtr msg){
    dist_right_ = msg->range;
  }
  //publica as distancias num array
  void timer_callback(){
    std_msgs::msg::Float64MultiArray msg;
    msg.data.resize(4);
    msg.data[0] = dist_front_;
    msg.data[1] = dist_rear_;
    msg.data[2] = dist_left_;
    msg.data[3] = dist_right_;

    distances_pub_->publish(msg);
  }

  double dist_front_;
  double dist_rear_;
  double dist_left_;
  double dist_right_;

  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr front_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr rear_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr left_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr right_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr distances_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

public:
  Sensors() : Node("sensors"),
              dist_front_(1.0),
              dist_rear_(1.0),
              dist_left_(1.0),
              dist_right_(1.0)
  {
    // aqui recebe as leituras do gazebo
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

    // e aqui publica pra central
    distances_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "/robot/distances", 10);

    // controle do timer pras publications
    timer_ = this->create_wall_timer(
      1000ms, std::bind(&Sensors::timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Nó sensors inicializado.");
  }
};
int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Sensors>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}