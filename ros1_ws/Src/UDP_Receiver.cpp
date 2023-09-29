#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>
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



int sockfd2;

struct sockaddr_in servaddr2, cliaddr;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("TEMP PUBLISH", 1000);
  ros::Rate loop_rate(10);

  memset(&servaddr2, 0, sizeof(servaddr2));
  memset(&cliaddr, 0, sizeof(cliaddr));
       
      // Filling server information
  servaddr2.sin_family    = AF_INET; // IPv4
  servaddr2.sin_addr.s_addr = htonl(INADDR_LOOPBACK);    
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

  int count = 0;
  while (ros::ok())
  {
    char buffer[MAXLINE];
      
    socklen_t len;
    int n;

   
    len = sizeof(cliaddr);  //len is value/result
   

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
    chatter_pub.publish(message);
    this->timer_->reset();

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}