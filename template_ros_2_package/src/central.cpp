#include <chrono>
#include <memory>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "geometry_msgs/msg/point.hpp"
#include "std_msgs/msg/bool.hpp"

using namespace std::chrono_literals;

class Central : public rclcpp::Node{

private:

  float dist_front_;
  float dist_rear_;
  float dist_left_;
  float dist_right_;
  
  bool turn_left_;

  bool red_msg;
  bool yellow_msg;

  // maquina de estados
  enum class State
  {
    FORWARD,
    SEARCHING
  };

  // callbacks dos sensores
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

  // callbacks da camera
  void red_callback(const std_msgs::msg::Bool::SharedPtr msg){
  red_msg = msg->data;
}
  void yellow_callback(const std_msgs::msg::Bool::SharedPtr msg){
  yellow_msg = msg->data;
  }

  // mudar estado e zerar o "timer" daquele estado
  void change_state(State new_state){
    state_ = new_state;
    state_start_time_ = this->get_clock()->now();
  }

  void control_loop(){

    float left = 0.0;
    float right = 0.0;

    const double turn_time = 2.90;   
    const float threshold = 0.35f; // distância mínima pra considerar obstáculo à frente
    const float red_threshold = 0.37f; // distância mínima pra considerar vermelho
    const float dest_threshold = 0.5f; // distancia do fim do labirinto para a parede amarela (tem que ser perto pq a camera so enxerga bem perto)

    auto now = this->get_clock()->now();
    double dt_state = (now - state_start_time_).seconds();

    switch (state_){
      case State::FORWARD:{
        if (dist_front_ < dest_threshold && yellow_msg){
          RCLCPP_INFO(this->get_logger(),
                      "Destino alcançado (%.2f m), parando!", dist_front_);
          left = 0.0;
          right = 0.0;

        } else if (dist_front_ > threshold){
          left = 3.0;
          right = 3.0;
        } else {
          // apareceu o obstaculo, agora vai decidir o lado pra virar
          RCLCPP_INFO(this->get_logger(),
                      "Parede à frente a %.2f m, escolhendo lado pra virar...", dist_front_);

          // decide uma vez so qual lado tem mais espaco
          if(dist_front_ <= red_threshold && red_msg){  
            turn_left_ = true;
            RCLCPP_INFO(this->get_logger(),
                        "tem vermelho na frente, virando pra esquerda");
          } else if (dist_left_ > dist_right_){
            turn_left_ = true;
            RCLCPP_INFO(this->get_logger(),
                        "A esquerda tem mais espaço, virando");
          } else if(dist_right_ > dist_left_){
            turn_left_ = false;
            RCLCPP_INFO(this->get_logger(),
                        "A direita tem mais espaço, virando");
          }
          change_state(State::SEARCHING);
        }
        break;
      }

      case State::SEARCHING:{
        if (turn_left_) {
          left = -1.5;
          right = 1.5;
        }else{
          left = 1.5;
          right = -1.5;
        }

        if (dt_state > turn_time){
          RCLCPP_INFO(this->get_logger(),
                      "Virou, segue o baile.");
          change_state(State::FORWARD);
          left = 2.0;
          right = 2.0;
        }
        break;
      }
    }
    
    cmd_left.data.clear();
    cmd_right.data.clear();
    cmd_left.data.push_back(left);
    cmd_right.data.push_back(right);

    motor1_pub_->publish(cmd_left);
    motor2_pub_->publish(cmd_right);
  }

  State state_;
  rclcpp::Time state_start_time_;
  
  std_msgs::msg::Float64MultiArray cmd_left;
  std_msgs::msg::Float64MultiArray cmd_right;

  rclcpp::Subscription<geometry_msgs::msg::Point>::SharedPtr cam_sub_;

  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor1_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr motor2_pub_;

  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr front_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr rear_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr left_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr right_sub_;

  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr red_sub_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr yellow_sub_;

  rclcpp::TimerBase::SharedPtr timer_;
  
public:
  Central() : Node("central"),
            state_(State::FORWARD),
            dist_front_(1.0),
            dist_rear_(1.0),
            dist_left_(1.0),
            dist_right_(1.0),
            turn_left_(true),
            red_msg(false),
            yellow_msg(false)
  {
    // publishers dos motores show
    motor1_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor1/commands", 10);
    motor2_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "motor2/commands", 10);

    // subscribers dos sensores show
    front_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/front", 10,
      std::bind(&Central::front_callback, this, std::placeholders::_1));

    rear_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/rear", 10,
      std::bind(&Central::rear_callback, this, std::placeholders::_1));

    left_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/left", 10,
      std::bind(&Central::left_callback, this, std::placeholders::_1));

    right_sub_ = this->create_subscription<sensor_msgs::msg::Range>(
      "/sensor_range/right", 10,
      std::bind(&Central::right_callback, this, std::placeholders::_1));

    red_sub_ = this->create_subscription<std_msgs::msg::Bool>(
    "/red_detected", 10,
    std::bind(&Central::red_callback, this, std::placeholders::_1));

    yellow_sub_ = this->create_subscription<std_msgs::msg::Bool>(
    "/yellow_detected", 10,
    std::bind(&Central::yellow_callback, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Nó central inicializado (com visão).");

    // timer da maquina de estados
    timer_ = this->create_wall_timer(100ms, std::bind(&Central::control_loop, this));

    state_start_time_ = this->get_clock()->now();

    RCLCPP_INFO(this->get_logger(), "Nó central inicializado.");
  }
};

int main(int argc, char ** argv){
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Central>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}