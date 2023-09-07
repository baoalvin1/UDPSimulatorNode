#include <ros/ros.h>
#include <PublisherSubscriber.h>
#include <std_msgs/String.h>

template<>
void PublisherSubscriber<std_msgs::String, std_msgs::String>::subscriberCallback(const std_msgs:: String::ConstPtr& recievedMsg)
{
    ROS_INFO("I recieved the following: %s", recievedMsg->data.c_str());
    ROS_INFO("Sending the recieved message on 'echo' topic");
    std_msgs:: String echo_msg;
    echo_msg.data = recievedMsg->data;
    publisherObject.publish(echo_msg);
}

int main(int argc, char **argv)
{
    //Set up ROS.
    ros::init(argc, argv, "publisher_subscriber_demo");
    PublisherSubscriber<std_msgs::String, std_msgs::String> parrot("echo", "chatter", 1);
    ros::spin();
}