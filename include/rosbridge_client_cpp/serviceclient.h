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
#ifndef ROSBRIDGE_CLIENT_CPP_SERVICECLIENT_H
#define ROSBRIDGE_CLIENT_CPP_SERVICECLIENT_H

#include "queues.h"
#include "rosbridgeclient.h"

namespace rosbridge_client_cpp
{

/**
 * Use this class to call a ROS service.
 * `ServiceClient::call()` method accepts as argument a json object or a vector of key-value pairs.
 *
 * Example with `picojson::object`:
 * @code
 *   ServiceClient my_client(rb, "/my/service");
 *   picojson::object json;
 *   json["data"] = picojson::value("Hello There!");
 *   my_client.call(json, seconds(5));
 * @endcode
 *
 * Example with `rosbridge_client_cpp::keys_values_t`:
 * @code
 *   ServiceClient my_client(rb, "/my/service");
 *   keys_values_t msg = { {"data", "Hello There!"} };
 *   my_client.call(msg, seconds(5));
 * @endcode
 *
 * @attention Rosbridge cannot guarantee that in case of disconnection the response of the service is catched:
 *            for this reason the method `call()` returns an optional type.
 *            Make sure you handle properly the case of optional without value when timeout occours.
 */
class ServiceClient
{
public:
    typedef std::unique_ptr<ServiceClient> uptr_t;

    ServiceClient(RosbridgeClient& rb, const std::string& service_, int queue_size_ = 1000);

    ~ServiceClient();

    template<typename T> std::optional<picojson::object> call(T input, std::chrono::seconds timeout);

    const std::string service;

private:
    std::atomic_bool shutdown_requested;
    TalkerQueue::shared_ptr talker_queue;
    ListenerQueue::shared_ptr listener_queue;
};

}

#endif // ROSBRIDGE_CLIENT_CPP_SERVICECLIENT_H
