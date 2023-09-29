#include "ros/ros.h"
#include "std_msgs/String.h"
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


#define PORT     8080
#define PORT2    8081
#define MAXLINE 62500
using std::placeholders::_1;
using namespace std::chrono_literals;


std::string msg_hold;
int sockfd1;
int sockfd2;
struct sockaddr_in     servaddr;
struct sockaddr_in servaddr2, cliaddr;
StringBuffer buffer_sending;

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
        
  msg_hold = msg->data;

  Document d;
  d.SetObject();

  rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

  Value value;
  value.SetString(msg_hold.c_str(), d.GetAllocator());

  d.AddMember("message",  value, allocator);


  StringBuffer buffer2;
  buffer_sending.Clear();
  Writer<StringBuffer> writer(buffer_sending);
  d.Accept(writer);

  std::cout << buffer_sending.GetString() << std::endl;
  std::string sendStr = buffer_sending.GetString();

  std::string sendStr = buffer_sending.GetString();
  sendto(sockfd1, sendStr.c_str(), strlen(sendStr.c_str()),
    0, (const struct sockaddr *) &servaddr, sizeof(servaddr));

}

int main(int argc, char **argv)
{
  
  ros::init(argc, argv, "listener");

  if ( (sockfd1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
          perror("socket creation failed"); 
          exit(EXIT_FAILURE);
      }
  
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  
  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("TEMP SUBSCRIPTION", 1000, chatterCallback);

  ros::spin();

  return 0;
}