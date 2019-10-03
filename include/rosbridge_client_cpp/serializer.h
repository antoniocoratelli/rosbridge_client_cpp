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
#pragma once

#include <map>
#include <random>
#include <set>

#include "utils.h"
#include "op.h"
#include "vendor/picojson.h"

namespace rosbridge_client_cpp
{

class Serializer
{
public:
    Serializer(OP op_, std::string id_ = unique())
    {
        this->json["op"] = picojson::value(std::to_string(op_));
        this->json["id"] = picojson::value(id_);
    }

    virtual ~Serializer() {}

    std::string id() const { return picojson::value(this->json).get("id").to_str(); }
    std::string serialized() const { return rosbridge_client_cpp::to_string(this->json); }

    bool operator==(const Serializer& other) const { return this->serialized() == other.serialized(); }
    bool operator!=(const Serializer& other) const { return not (*this == other); }

protected:
    picojson::object json;
};

struct SerializerTopicAdvertise : public Serializer
{
    SerializerTopicAdvertise(std::string topic_, std::string type_):
        Serializer(OP::TopicAdvertiseOp)
    {
        this->json["topic"] = picojson::value(topic_);
        this->json["type"] = picojson::value(type_);
    }
};

struct SerializerTopicUnadvertise : public Serializer
{
    SerializerTopicUnadvertise(std::string topic_):
        Serializer(OP::TopicUnadvertiseOp)
    {
        this->json["topic"] = picojson::value(topic_);
    }
};

struct SerializerTopicMessage: public Serializer
{
    SerializerTopicMessage(std::string topic_, picojson::object msg_):
        Serializer(OP::TopicPublishOp)
    {
        this->json["topic"] = picojson::value(topic_);
        this->json["msg"] = picojson::value(msg_);
    }
};

struct SerializerTopicSubscribe: public Serializer
{
    SerializerTopicSubscribe(std::string topic_, std::string type_, int queue_length_):
        Serializer(OP::TopicSubscribeOp)
    {
        this->json["topic"] = picojson::value(topic_);
        this->json["type"] = picojson::value(type_);
        this->json["queue_length"] = picojson::value(static_cast<double>(queue_length_));
    }
};

struct SerializerTopicUnsubscribe: public Serializer
{
    SerializerTopicUnsubscribe(std::string topic_):
        Serializer(OP::TopicUnsubscribeOp)
    {
        this->json["topic"] = picojson::value(topic_);
    }
};

struct SerializerServiceAdvertise: public Serializer
{
    SerializerServiceAdvertise(std::string service_, std::string type_):
        Serializer(OP::ServiceAdvertiseOp)
    {
        this->json["service"] = picojson::value(service_);
        this->json["type"] = picojson::value(type_);
    }
};

struct SerializerServiceUnadvertise: public Serializer
{
    SerializerServiceUnadvertise(std::string service_):
        Serializer(OP::ServiceUnadvertiseOp)
    {
        this->json["service"] = picojson::value(service_);
    }
};

struct SerializerServiceRequest: public Serializer
{
    SerializerServiceRequest(std::string service_, picojson::object args_):
        Serializer(OP::ServiceCallOp)
    {
        this->json["service"] = picojson::value(service_);
        this->json["args"] = picojson::value(args_);
    }
};

struct SerializerServiceResponse: public Serializer
{
    SerializerServiceResponse(std::string service_, picojson::object values_, bool result_, std::string id_):
        Serializer(OP::ServiceResponseOp, id_)
    {
        this->json["service"] = picojson::value(service_);
        this->json["result"] = picojson::value(result_);
        this->json["values"] = picojson::value(values_);
    }
};

} // namespace rosbridge_client_cpp