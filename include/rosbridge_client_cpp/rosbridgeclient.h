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
#ifndef ROSBRIDGE_CLIENT_CPP_ROSBRIDGECLIENT_H
#define ROSBRIDGE_CLIENT_CPP_ROSBRIDGECLIENT_H

#include <atomic>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include "connection.h"
#include "utils.h"
#include "queues.h"

namespace rosbridge_client_cpp
{

/**
 * RosbridgeClient is the handler .
 * Only one RosbridgeClient should declared in the application, but is not a strict requirement.
 */
class RosbridgeClient
{
public:

    /// Typedef utilities
    /// @{
    typedef std::unique_ptr<RosbridgeClient> uptr_t;
    typedef std::function<void(void)> callback_t;
    /// @}

    /**
     * Instanciates a RosbridgeClient.
     * @param address_ Ip address or hostname of the RosbridgeServer.
     * @param port_ Port number of the RosbridgeServer.
     * @param on_connection_ Callback executed every time the client initializes a connection to the server.
     * @param on_disconnection_ Callback executed every time the connection to the server is dropped.
     * @attention on_connection_ and on_disconnection_ are NOT guaranteed to be called in order, and NOT guaranteed
     *            to be called just once. For example, if the server is connected and then disconnected, the callback
     *            on_disconnection_ could be called three times in a row in the following seconds.
     */
    RosbridgeClient(std::string address_, int port_, callback_t on_connection_, callback_t on_disconnection_);

    ~RosbridgeClient();

    /// Methods to register queues.
    /// @attention These methods are used internally, do NOT use them in your application.
    /// @{
    void attach(TalkerQueue::shared_ptr publisher);
    void attach(ListenerQueue::shared_ptr p, std::string topic);
    /// @}

private:

    Connection connection;
    Timer talker_timer;
    std::vector<TalkerQueue::shared_ptr> talkers;
    std::map<std::string, std::vector<ListenerQueue::shared_ptr>> listeners;

    void resetTalkers();
    void talk();
    void listen(const std::string& data);
};

} // rosbridge_client_cpp

#endif // ROSBRIDGE_CLIENT_CPP_ROSBRIDGECLIENT_H
