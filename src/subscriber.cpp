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
#include <rosbridge_client_cpp/subscriber.h>

namespace rosbridge_client_cpp
{

Subscriber::Subscriber(RosbridgeClient& rb, const std::string& topic_, const std::string& type_, TopicCallback callback_, int queue_size_):
    topic(topic_), type(type_), callback(callback_), shutdown_requested(false)
{
    SerializerTopicSubscribe subscribe_msg(topic, type, queue_size_);
    this->talker_queue.reset(new TalkerQueue(10, subscribe_msg)),
    this->listener_queue.reset(new ListenerQueue(queue_size_));
    RBC_LOG_DEBUG("{"
        << "topic=" << topic << ", type=" << type << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    rb.attach(this->talker_queue);
    rb.attach(this->listener_queue, this->topic);
    this->listener_thread = std::thread(&Subscriber::listener, this);
}

rosbridge_client_cpp::Subscriber::~Subscriber()
{
    this->shutdown_requested.store(true);
    SerializerTopicUnsubscribe unsubscribe_msg(topic);
    this->talker_queue->push(unsubscribe_msg);
    if (this->listener_thread.joinable()) this->listener_thread.join();
    RBC_LOG_DEBUG("{"
        << "topic=" << topic << ", type=" << type << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    this->talker_queue->abandon();
    this->listener_queue->abandon();
}

void rosbridge_client_cpp::Subscriber::listener()
{
    while (not this->shutdown_requested.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (not this->listener_queue->empty())
        {
            Unserializer msg = this->listener_queue->next();
            this->callback(msg.payload());
            this->listener_queue->pop();
        }
    }
}

} // namespace rosbridge_client_cpp
