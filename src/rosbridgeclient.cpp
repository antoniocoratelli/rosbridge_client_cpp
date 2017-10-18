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
#include <rosbridge_client_cpp/rosbridgeclient.h>

using namespace rosbridge_client_cpp;

RosbridgeClient::RosbridgeClient(std::string address_, int port_, callback_t on_connection_, callback_t on_disconnection_):
    connection(address_, port_)
{
    RBC_LOG_INFO("Hello!");
    this->connection.on_connection = [this, on_connection_] { this->resetTalkers(); on_connection_(); };
    this->connection.on_disconnection = on_disconnection_;
    this->connection.on_message_received = [this](const std::string& d) { this->listen(d); };
    this->talker_timer.start(5, [this]() { this->talk(); });
}

RosbridgeClient::~RosbridgeClient()
{
    this->connection.on_connection = []{};
    this->connection.on_disconnection = []{};
    this->connection.on_message_received = [](const std::string&){};
    RBC_LOG_INFO("Goodbye!");
}

void RosbridgeClient::attach(TalkerQueue::shared_ptr q)
{
    RBC_LOG_DEBUG("TalkerQueue{id=" << q.get() << "}");
    this->talkers.push_back(q);
}

void RosbridgeClient::attach(ListenerQueue::shared_ptr q, std::string topic)
{
    RBC_LOG_DEBUG("ListenerQueue{id=" << q.get() << "}");
    this->listeners[topic].push_back(q);
}

void RosbridgeClient::listen(const std::string& data)
{
    Unserializer unserializer(data);
    switch (unserializer.type())
    {
    case OP::TopicPublishOp:
        for (ListenerQueue::shared_ptr& listener : this->listeners[unserializer.topic()])
            listener->push(unserializer);
        return;
    case OP::ServiceCallOp:
        for (ListenerQueue::shared_ptr& listener : this->listeners[unserializer.service()])
            listener->push(unserializer);
        return;
    case OP::ServiceResponseOp:
        for (ListenerQueue::shared_ptr& listener : this->listeners[unserializer.service()])
            listener->push(unserializer);
        return;
    }
    RBC_LOG_WARN("OPERATION NOT IMPLEMENTED " << data);
}

void RosbridgeClient::talk()
{
    for(TalkerQueue::shared_ptr& t : this->talkers)
    {
        if (not this->connection.active()) break;
        if (t->empty()) continue;
        this->connection.send(t->next().serialized());
        t->pop();
    }
}

void RosbridgeClient::resetTalkers()
{
    RBC_LOG_INFO("Resetting talkers ...");
    for(TalkerQueue::shared_ptr& t : this->talkers) t->reset();
}
