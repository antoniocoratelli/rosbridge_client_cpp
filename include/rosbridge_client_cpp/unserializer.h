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

class Unserializer
{
public:

    explicit Unserializer(std::string message_)
    {
        picojson::value v;
        std::string err = picojson::parse(v, message_);
        this->json = v.get<picojson::object>();
        if (not err.empty())
            RBC_LOG_WARN("Could not unserialize: " << err << " -- MSG " << message_);
    }

    Unserializer(const Unserializer& other)
    {
        this->json = other.json;
    }

    std::string id()
    {
        if (not this->json["id"].is<std::string>()) return "";
        return this->json["id"].get<std::string>();
    }

    OP type()
    {
        std::string value = this->json["op"].get<std::string>();
        return stringToOP(value);
    }

    bool result()
    {
        if (this->type() != OP::ServiceResponseOp)
            throw std::runtime_error("Message of this type does not contain 'result' field.");
        return this->json["result"].get<bool>();
    }

    std::string topic()
    {
        if (not isTopicOP(this->type()))
            throw std::runtime_error("Message of this type does not contain 'topic' field.");
        return this->json["topic"].get<std::string>();
    }

    std::string service()
    {
        if (not isServiceOP(this->type()))
            throw std::runtime_error("Message of this type does not contain 'service' field.");
        return this->json["service"].get<std::string>();
    }

    std::string data()
    {
        return rosbridge_client_cpp::to_string(this->json);
    }

    picojson::object payload()
    {
        if (OP::TopicPublishOp == this->type())
            return this->json["msg"].get<picojson::object>();
        if (OP::ServiceCallOp == this->type())
            return this->json["args"].get<picojson::object>();
        if (OP::ServiceResponseOp == this->type())
            return this->json["values"].get<picojson::object>();
        return picojson::object();
    }

    bool operator==(const Unserializer& other) const { return this->json == other.json; }
    bool operator!=(const Unserializer& other) const { return not (*this == other); }

private:
    picojson::object json;
};

} // namespace rosbridge_client_cpp