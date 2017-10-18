#include <gtest/gtest.h>

#include <ros/ros.h>
#include <std_msgs/String.h>

#include <rosbridge_client_cpp/rosbridge.h>

class TestPublisher : public ::testing::Test
{
public:
    TestPublisher()
    {
        this->subscriber = nh.subscribe(topic, 10, &TestPublisher::callback, this);
    }

    void publish()
    {
        rosbridge_client_cpp::RosbridgeClient rb("localhost", 9090, []{}, []{});
        rosbridge_client_cpp::Publisher publisher(rb, topic, "std_msgs/String", 10);
        picojson::object json;
        json["data"] = picojson::value(expected);
        publisher.publish(json);
    }

    void callback(const std_msgs::String& msg) { if (expected == msg.data) this->message_received = true; }

    const std::string topic = "/rosbridge_client_cpp/integration_tests/publisher";
    const std::string expected = "Hello rosbridge_client_cpp";

    ros::NodeHandle nh;
    ros::Subscriber subscriber;
    bool message_received = false;
};

TEST_F(TestPublisher, publisher)
{
    this->publish();
    for(int i = 0; i < 10; i++)
    {
        ros::spin();
        ros::Duration(0.1).sleep();
    }
    ASSERT_EQ(this->subscriber.getNumPublishers(), 0U);
    ASSERT_TRUE(this->message_received);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  ros::init(argc, argv, "rosbridge_client_cpp_integration_tests");
  ros::NodeHandle nh;
  return RUN_ALL_TESTS();
}
