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
#include <rosbridge_client_cpp/publisher.h>

namespace rosbridge_client_cpp
{

Publisher::Publisher(RosbridgeClient& rb, const std::string& topic_, const std::string& type_, int queue_size_):
    topic(topic_), type(type_)
{
    SerializerTopicAdvertise advertise_msg(this->topic, this->type);
    this->talker_queue.reset(new TalkerQueue(queue_size_, advertise_msg));
    RBC_LOG_DEBUG("{"
        << "topic=" << topic << ", type=" << type << ", talker_queue=" << this->talker_queue.get() << "}");
    rb.attach(this->talker_queue);
}

rosbridge_client_cpp::Publisher::~Publisher()
{
    rosbridge_client_cpp::SerializerTopicUnadvertise unadvertise_msg(topic);
    this->talker_queue->push(unadvertise_msg);
    RBC_LOG_DEBUG("{"
        << "topic=" << topic << ", type=" << type << ", talker_queue=" << this->talker_queue.get() << "}");
    this->talker_queue->abandon();
}

template<>
void Publisher::publish(picojson::object json)
{
    SerializerTopicMessage msg(this->topic, json);
    this->talker_queue->push(msg);
}

template<>
void Publisher::publish(keys_values_t keys_values)
{
    this->publish(to_json(keys_values));
}

} // namespace rosbridge_client_cpp
