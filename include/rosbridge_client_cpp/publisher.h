/*
 * Copyright (C) 2017 Antonio Coratelli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ROSBRIDGE_CLIENT_CPP_PUBLISHER_H
#define ROSBRIDGE_CLIENT_CPP_PUBLISHER_H

#include <memory>
#include <queue>

#include "utils.h"
#include "serializer.h"
#include "rosbridgeclient.h"
#include "queues.h"
#include "unserializer.h"
#include "vendor/picojson.h"

namespace rosbridge_client_cpp
{

/**
 * Use this class to publish data on a ROS topic.
 * `Publisher::publish()` method accepts as argument a json object or a vector of key-value pairs.
 *
 * Example with `picojson::object`:
 * @code
 *   Publisher my_pub(rb, "/my/topic", "std_msgs/String", 20);
 *   picojson::object json;
 *   json["data"] = picojson::value("Hello There!");
 *   my_pub.publish<picojson::object>(json);
 * @endcode
 *
 * Example with `rosbridge_client_cpp::keys_values_t`:
 * @code
 *   Publisher my_pub(rb, "/my/topic", "std_msgs/String", 20);
 *   keys_values_t msg = { {"data", "Hello There!"} };
 *   my_pub.publish(msg);
 * @endcode
 */
class Publisher
{
public:
    typedef std::unique_ptr<Publisher> uptr_t;

    Publisher(RosbridgeClient& rb, const std::string& topic_, const std::string& type_, int queue_size_ = 1);

    ~Publisher();

    template<typename T> void publish(T data);

    const std::string topic;
    const std::string type;

private:
    TalkerQueue::shared_ptr talker_queue;
};

} // namespace rosbridge_client_cpp

#endif // ROSBRIDGE_CLIENT_CPP_PUBLISHER_H
