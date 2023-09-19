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
using namespace rapidjson;
   
#define PORT     8080
#define PORT2    8081
#define MAXLINE 62500
using std::placeholders::_1;
using namespace std::chrono_literals;

std::string msg_hold;
int sockfd1;
int sockfd2;

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

      auto sub_opt = rclcpp::SubscriptionOptions();
      sub_opt.callback_group = callback_group_subscriber_;

      subscription_ = this->create_subscription<std_msgs::msg::String>(
      "topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1), sub_opt);
      publisher_ = this->create_publisher<std_msgs::msg::String>("echo", 1);
      timer_ = this->create_wall_timer(
      1000ms, std::bind(&MinimalSubscriber::timer_callback, this), callback_group_publisher_);
      
    }

  private:
    void topic_callback(const std_msgs::msg::String & msg) const
    {
      // int sockfd;
      // char buffer[MAXLINE];
      struct sockaddr_in     servaddr;
   
      // Creating socket file descriptor
      if ( (sockfd1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
          perror("socket creation failed"); 
          exit(EXIT_FAILURE);
      }
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
      msg_hold = msg.data;

      Document d;
      d.SetObject();

      rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

      // // size_t sz = allocator.Size();
      Value value;
      value.SetString(msg_hold.c_str(), d.GetAllocator());

      d.AddMember("message",  value, allocator);
      // d.AddMember("testId",   2, allocator);
      // d.AddMember("group",    3, allocator);
      // d.AddMember("order",    4, allocator);

    // const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    // Document d;
    // d.Parse(json);

    // // 2. Modify it by DOM.
    // Value& s = d["stars"];
    // s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
      StringBuffer buffer2;
      Writer<StringBuffer> writer(buffer2);
      d.Accept(writer);

      std::cout << buffer2.GetString() << std::endl;
      std::string sendStr = buffer2.GetString();
   
      memset(&servaddr, 0, sizeof(servaddr));

      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(PORT);
      servaddr.sin_addr.s_addr = INADDR_ANY;
       
      // int n;
      // socklen_t len;
       
      sendto(sockfd1, sendStr.c_str(), strlen(sendStr.c_str()),
          0, (const struct sockaddr *) &servaddr, 
              sizeof(servaddr));
      // std::cout<<"Hello message sent."<<std::endl;
    }
    void timer_callback()
    {
      // int sockfd;
      char buffer[MAXLINE];
      struct sockaddr_in servaddr, cliaddr;
       
      // Creating socket file descriptor
      if ( (sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
          perror("socket creation failed");
          exit(EXIT_FAILURE);
      }
       
      memset(&servaddr, 0, sizeof(servaddr));
      memset(&cliaddr, 0, sizeof(cliaddr));
       
      // Filling server information
      servaddr.sin_family    = AF_INET; // IPv4
      servaddr.sin_addr.s_addr = INADDR_ANY;
      servaddr.sin_port = htons(PORT2);

      int optval = 1;
      setsockopt(sockfd2, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
       
      // Bind the socket with the server address
      if ( bind(sockfd2, (const struct sockaddr *)&servaddr, 
          sizeof(servaddr)) < 0 )
      {
          perror("bind failed");
          exit(EXIT_FAILURE);
      }
       
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
      buffer[n] = '\0';

      Document d;
      d.Parse(buffer);

      StringBuffer buffer2;
      Writer<StringBuffer> writer(buffer2);
      d.Accept(writer);

      std::cout << buffer2.GetString() << std::endl;
      auto message = std_msgs::msg::String();
      message.data = "Hello, world! " + std::string(buffer2.GetString());
      RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
      publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::CallbackGroup::SharedPtr callback_group_subscriber_;
    rclcpp::CallbackGroup::SharedPtr callback_group_publisher_;
    size_t count_;
};

int main(int argc, char * argv[])
{
  
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MinimalSubscriber>();
  rclcpp::executors::MultiThreadedExecutor exec;
  exec.add_node(node);

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