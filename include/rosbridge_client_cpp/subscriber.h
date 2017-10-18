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
#ifndef ROSBRIDGE_CLIENT_CPP_SUBSCRIBER_H
#define ROSBRIDGE_CLIENT_CPP_SUBSCRIBER_H

#include <atomic>
#include "rosbridgeclient.h"
#include "queues.h"

namespace rosbridge_client_cpp
{

/**
 * Use this class to subscribe a ROS topic.
 * @attention The callback is executed in a separate thread, please make sure that your callback is thread-safe.
 *
 * Example:
 * @code
 *   auto my_callback = [](const picojson::object& json) {std::cout << std::to_string(json) << std::endl;};
 *   Subscriber my_sub(rb, "/my/topic", "std_msgs/String", my_callback, 5);
 * @endcode
 */
class Subscriber
{
public:
    typedef std::unique_ptr<Subscriber> uptr_t;

    Subscriber(RosbridgeClient& rb, const std::string& topic_, const std::string& type_, TopicCallback callback_, int queue_size_ = 10);

    ~Subscriber();

    const std::string topic;
    const std::string type;

private:
    TopicCallback callback;
    std::atomic_bool shutdown_requested;
    TalkerQueue::shared_ptr talker_queue;
    ListenerQueue::shared_ptr listener_queue;
    std::thread listener_thread;

    void listener();
};

} // namespace rosbridge_client_cpp

#endif // ROSBRIDGE_CLIENT_CPP_SUBSCRIBER_H
