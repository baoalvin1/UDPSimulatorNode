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
#define MAXLINE 62500
using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber")
    {
      subscription_ = this->create_subscription<std_msgs::msg::String>(
      "topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));
    }

  private:
    void topic_callback(const std_msgs::msg::String & msg) const
    {
      RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg.data.c_str());
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  int sockfd;
  // char buffer[MAXLINE];
  struct sockaddr_in     servaddr;
   
  // Creating socket file descriptor
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
      perror("socket creation failed"); 
      exit(EXIT_FAILURE);
  }
  rclcpp::init(argc, argv);
  while(true) {
    rclcpp::spin(std::make_shared<MinimalSubscriber>());

    // Document d;
    // d.SetObject();

    // rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

    // // size_t sz = allocator.Size();

    // d.AddMember("version",  1, allocator);
    // d.AddMember("testId",   2, allocator);
    // d.AddMember("group",    3, allocator);
    // d.AddMember("order",    4, allocator);

    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

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
       
    sendto(sockfd, sendStr.c_str(), strlen(sendStr.c_str()),
        0, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    std::cout<<"Hello message sent."<<std::endl;
    close(sockfd);

    rclcpp::shutdown();
  }
  return 0;
}