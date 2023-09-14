#ifndef PUBLISHER_SUBSCRIBER_H
#define PUBLISHER_SUBSCRIBER_H

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

template<typename PublishT, typename SubscribeT>
class PublisherSubscriber : public rclcpp::Node
{
public:
    PublisherSubscriber() : Node("publisher_subscriber_node") {}
    PublisherSubscriber(std::string publishTopicName, std::string subscribeTopicName, int queueSize)
        : Node("publisher_subscriber_node")
    {
        publisherObject = this->create_publisher<PublishT>(publishTopicName, queueSize);
        subscriberObject = this->create_subscription<SubscribeT>(
            subscribeTopicName, queueSize, [this](const SubscribeT::SharedPtr msg) {
                this->subscriberCallback(msg);
            }
        );
    }

    void subscriberCallback(const typename SubscribeT::SharedPtr& receivedMsg)
    {
        RCLCPP_INFO(this->get_logger(), "I received the following: %s", receivedMsg->data.c_str());
        RCLCPP_INFO(this->get_logger(), "Sending the received message on 'echo' topic");
        auto echo_msg = std::make_shared<PublishT>();
        echo_msg->data = receivedMsg->data;
        publisherObject->publish(*echo_msg);
    }

protected:
    rclcpp::Subscription<SubscribeT>::SharedPtr subscriberObject;
    rclcpp::Publisher<PublishT>::SharedPtr publisherObject;
};

#endif
