// Client side implementation of UDP client-server model
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;
   
#define PORT     8081
#define MAXLINE 62500
   
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    const char *hello = "Hello from client";
    struct sockaddr_in     servaddr;
   
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // ondemand::parser parser;
    // padded_string json = padded_string::load("twitter.json");
    // ondemand::document tweets = parser.iterate(json);
    // std::cout << uint64_t(tweets["search_metadata"]["count"]) << " results." << std::endl;

    // std::string_view str = simdjson::to_json_string(tweets["search_metadata"]);
    // std::cout << str << std::endl;
    // const char* strcp = std::string(str).c_str();

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


    std::string sendStr = buffer2.GetString();
    // // Output {"project":"rapidjson","stars":11}
    // std::cout << buffer.GetString() << std::endl;
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);;
       
    int n;
    socklen_t len;
       
    sendto(sockfd, sendStr.c_str(), strlen(sendStr.c_str()),
        0, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    std::cout<<"Hello message sent."<<std::endl;
           
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    std::cout<<"Server :"<<buffer<<std::endl;
   
    close(sockfd);
    return 0;
}