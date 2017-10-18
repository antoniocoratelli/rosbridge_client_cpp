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
#include <rosbridge_client_cpp/serviceserver.h>

rosbridge_client_cpp::ServiceServer::ServiceServer(
        RosbridgeClient& rb, const std::string& service_, const std::string& type_, ServiceServerCallback callback_, int queue_size_
    ):
    service(service_),
    type(type_),
    callback(callback_),
    shutdown_requested(false)
{
    SerializerServiceAdvertise advertise_msg(this->service, this->type);
    this->talker_queue.reset(new TalkerQueue(queue_size_, advertise_msg));
    this->listener_queue.reset(new ListenerQueue(queue_size_));
    RBC_LOG_DEBUG("{"
        << "service=" << service << ", type=" << type << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    rb.attach(this->talker_queue);
    rb.attach(this->listener_queue, this->service);
    this->listener_thread = std::thread(&ServiceServer::listener, this);
}

rosbridge_client_cpp::ServiceServer::~ServiceServer()
{
    this->shutdown_requested.store(true);
    SerializerServiceUnadvertise unadvertise_msg(this->service);
    this->talker_queue->push(unadvertise_msg);
    if (this->listener_thread.joinable()) this->listener_thread.join();
    RBC_LOG_DEBUG("{"
        << "service=" << service << ", type=" << type << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    this->talker_queue->abandon();
    this->listener_queue->abandon();
}

void rosbridge_client_cpp::ServiceServer::listener()
{
    while (not this->shutdown_requested.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (this->listener_queue->empty()) continue;
        Unserializer msg = this->listener_queue->next();
        try
        {
            picojson::object response = this->callback(msg.payload());
            SerializerServiceResponse response_msg(this->service, response, true, msg.id());
            this->talker_queue->push(response_msg);
        }
        catch (...) { }
        this->listener_queue->pop();
    }
}
