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
#ifndef ROSBRIDGE_CLIENT_CPP_SERVICESERVER_H
#define ROSBRIDGE_CLIENT_CPP_SERVICESERVER_H

#include "queues.h"
#include "rosbridgeclient.h"

namespace rosbridge_client_cpp
{

/**
 * Use this class to provide a ROS service.
 * @attention The callback is executed in a separate thread, please make sure that your callback is thread-safe.
 *
 * Example:
 * @code
 *   auto my_callback = [](const picojson::object& request) -> picojson::object
 *   {
 *     picojson::object response;
 *     response["success"] = picojson::value(true);
 *     response["message"] = picojson::value(unique());
 *     return response;
 *   };
 *   ServiceServer my_server("/my/service", "std_srvs/Trigger", my_callback);
 * @endcode
 */
class ServiceServer
{
public:
    typedef std::unique_ptr<ServiceServer> uptr_t;

    ServiceServer(
        RosbridgeClient& rb, const std::string& service_, const std::string& type_, ServiceServerCallback callback_, int queue_size_ = 100
    );

    ~ServiceServer();

    const std::string service;
    const std::string type;

private:
    ServiceServerCallback callback;
    std::atomic_bool shutdown_requested;
    TalkerQueue::shared_ptr talker_queue;
    ListenerQueue::shared_ptr listener_queue;

    std::thread listener_thread;

    void listener();
};

}

#endif // ROSBRIDGE_CLIENT_CPP_SERVICESERVER_H
