#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "custom_messages/msg/real_vehicle_state.hpp"
using namespace rapidjson;
   
#define PORT     8081
#define PORT2    8080
#define MAXLINE 62500
using std::placeholders::_1;
using namespace std::chrono_literals;

std::string msg_hold;
int sockfd1;
int sockfd2;
struct sockaddr_in     servaddr;
struct sockaddr_in servaddr2, cliaddr;
StringBuffer buffer_sending;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber"), count_(0)
    {
      callback_group_publisher_ = this->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);

      callback_group_subscriber_ = this->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);

      callback_group_timer2_ = this->create_callback_group(
      rclcpp::CallbackGroupType::MutuallyExclusive);

      auto sub_opt = rclcpp::SubscriptionOptions();
      sub_opt.callback_group = callback_group_subscriber_;

      subscription_ = this->create_subscription<std_msgs::msg::String>(
      "topic 1", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1), sub_opt);
      subscription2_ = this->create_subscription<std_msgs::msg::String>(
      "topic 2", 10, std::bind(&MinimalSubscriber::topic_callback2, this, _1), sub_opt);
      subscription3_ = this->create_subscription<std_msgs::msg::String>(
      "topic 3", 10, std::bind(&MinimalSubscriber::topic_callback3, this, _1), sub_opt);
      publisher_ = this->create_publisher<custom_messages::msg::RealVehicleState>("real_vehicle_outputs", 1);
      timer_ = this->create_wall_timer(
      20ms, std::bind(&MinimalSubscriber::timer_callback, this), callback_group_publisher_);
      timer2_ = this->create_wall_timer(
      20ms, std::bind(&MinimalSubscriber::timer2_callback, this), callback_group_timer2_);
    }

  private:
    void topic_callback(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
      msg_hold = msg.data;

      Document d;
      d.SetObject();

      rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

      // // size_t sz = allocator.Size();
      Value value;
      value.SetString(msg_hold.c_str(), d.GetAllocator());

      d.AddMember("message",  value, allocator);

      buffer_sending.Clear();
      Writer<StringBuffer> writer(buffer_sending);
      d.Accept(writer);


      std::cout << buffer_sending.GetString() << std::endl;
      std::string sendStr = buffer_sending.GetString();
    }

    void topic_callback2(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
      msg_hold = msg.data;

      Document d;
      d.SetObject();

      rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

      // // size_t sz = allocator.Size();
      Value value;
      value.SetString(msg_hold.c_str(), d.GetAllocator());

      d.AddMember("message",  value, allocator);

      buffer_sending.Clear();
      Writer<StringBuffer> writer(buffer_sending);
      d.Accept(writer);


      std::cout << buffer_sending.GetString() << std::endl;
      std::string sendStr = buffer_sending.GetString();
    }

    void topic_callback3(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
      msg_hold = msg.data;

      Document d;
      d.SetObject();

      rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

      // // size_t sz = allocator.Size();
      Value value;
      value.SetString(msg_hold.c_str(), d.GetAllocator());

      d.AddMember("message",  value, allocator);

      buffer_sending.Clear();
      Writer<StringBuffer> writer(buffer_sending);
      d.Accept(writer);


      std::cout << buffer_sending.GetString() << std::endl;
      std::string sendStr = buffer_sending.GetString();
    }

    void timer_callback()
    {
      this->timer_->cancel();
      RCLCPP_INFO(this->get_logger(), "Timer Started");
      // int sockfd;
      char buffer[MAXLINE];
      
      socklen_t len;
      int n;

      // fd_set rfds;

      // FD_ZERO(&rfds);
      // FD_SET(socketHandle, &rfds);
      // recVal = select(socketHandle + 1, &rfds, NULL, NULL, &tv);
   
      len = sizeof(cliaddr);  //len is value/result
   
      // struct timeval read_timeout;
      // read_timeout.tv_sec = 0;
      // read_timeout.tv_usec = 10;
      // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
      n = recvfrom(sockfd2, (char *)buffer, MAXLINE, 
                SOCK_NONBLOCK, ( struct sockaddr *) &cliaddr,
                &len);
      RCLCPP_INFO(this->get_logger(), "Message Received");
      buffer[n] = '\0';

      Document d;
      d.Parse(buffer);


      StringBuffer buffer2;
      Writer<StringBuffer> writer(buffer2);
      d.Accept(writer);

      
      auto message = custom_messages::msg::RealVehicleState();
      message.vx = d["vx"].GetDouble();
      message.vy = d["vy"].GetDouble();
      message.x = d["x"].GetDouble();
      message.y = d["y"].GetDouble();
      message.psi = d["psi"].GetDouble();
      message.wz = d["wz"].GetDouble();
      std::cout << buffer2.GetString() << std::endl;

      // auto message = std_msgs::msg::String();
      // message.data = "Hello, world! " + std::string(buffer2.GetString());
      // RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      publisher_->publish(message);
      this->timer_->reset();
    }

    void timer2_callback() {
      std::string sendStr = buffer_sending.GetString();
      sendto(sockfd1, sendStr.c_str(), strlen(sendStr.c_str()),
          0, (const struct sockaddr *) &servaddr, 
              sizeof(servaddr));
    }


    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::TimerBase::SharedPtr timer2_;
    rclcpp::Publisher<custom_messages::msg::RealVehicleState>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription2_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription3_;
    rclcpp::CallbackGroup::SharedPtr callback_group_subscriber_;
    rclcpp::CallbackGroup::SharedPtr callback_group_publisher_;
    rclcpp::CallbackGroup::SharedPtr callback_group_timer2_;
    size_t count_;
};

int main(int argc, char * argv[])
{
  
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MinimalSubscriber>();
  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(node);

  if ( (sockfd1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
          perror("socket creation failed"); 
          exit(EXIT_FAILURE);
      }
  
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  // Creating socket file descriptor
  if ( (sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
      perror("socket creation failed");
      exit(EXIT_FAILURE);
  }

  
       
  
       
  memset(&servaddr2, 0, sizeof(servaddr2));
  memset(&cliaddr, 0, sizeof(cliaddr));
       
      // Filling server information
  servaddr2.sin_family    = AF_INET; // IPv4
  servaddr2.sin_addr.s_addr = inet_addr("192.168.2.100"); 
  servaddr2.sin_port = htons(PORT2);

  int optval = 1;
  setsockopt(sockfd2, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
       
      // Bind the socket with the server address
  if ( bind(sockfd2, (const struct sockaddr *)&servaddr2, 
      sizeof(servaddr2)) < 0 )
  {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }
       

  // while(rclcpp::ok()) {
  //   exec.spin_once();
  // }
  exec.spin();
  // close(sockfd);
  rclcpp::shutdown();
  close(sockfd1);
  close(sockfd2);
  exit(0);
  return 0;
}