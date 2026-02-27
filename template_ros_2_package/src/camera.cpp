#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/bool.hpp"

#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>

using namespace std::chrono_literals;

class CameraNode : public rclcpp::Node{

  public:

  CameraNode() : Node("camera"){
    // assinatura do tópico da câmera
    image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/frente_camera/frente_camera_sensor/image_raw",
      10,
      std::bind(&CameraNode::imageCallback, this, std::placeholders::_1));

    // publisher lógico de "viu vermelho"
    red_pub_ = this->create_publisher<std_msgs::msg::Bool>(
      "/red_detected", 10);

    yellow_pub_ = this->create_publisher<std_msgs::msg::Bool>(
      "/yellow_detected", 10);

    RCLCPP_INFO(this->get_logger(), "Camera iniciada.");
  }

private:

  void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg){
    
    cv::Mat bgr;

    // converter do ros pro opencv
      cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      bgr = cv_ptr->image;
 
    // Converter pra HSV
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    // Faixas do VERMELHO no HSV
    cv::Scalar lower_red1(0, 120, 70);
    cv::Scalar upper_red1(10, 255, 255);

    cv::Scalar lower_red2(170, 120, 70);
    cv::Scalar upper_red2(180, 255, 255);

    cv::Mat mask1, mask2, red_mask;
    cv::inRange(hsv, lower_red1, upper_red1, mask1);
    cv::inRange(hsv, lower_red2, upper_red2, mask2);

    red_mask = mask1 | mask2;

    cv::Scalar lower_yellow(20, 100, 100); // faixas do amarelo pra hsv
    cv::Scalar upper_yellow(30, 255, 255);

    std_msgs::msg::Bool red_msg; // Montar mensagem de "viu vermelho"

    cv::Mat yellow_mask;
    cv::inRange(hsv, lower_yellow, upper_yellow, yellow_mask);

    cv::Moments m = cv::moments(red_mask, true);  // Calcular momentos da máscara → área e centróide

    const double AREA_THRESHOLD = 2000.0;   // ajuste conforme sua câmera
    bool detectedr = (m.m00 > AREA_THRESHOLD);

    red_msg.data = detectedr;
    red_pub_->publish(red_msg);

    cv::Moments my = cv::moments(yellow_mask, true);

    bool detectedy = (my.m00 > AREA_THRESHOLD);
    std_msgs::msg::Bool yellow_msg;

    yellow_msg.data = detectedy;
    yellow_pub_->publish(yellow_msg);
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr red_pub_;
  rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr yellow_pub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CameraNode>());
  rclcpp::shutdown();
  return 0;
}