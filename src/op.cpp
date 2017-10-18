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
#include <rosbridge_client_cpp/op.h>

rosbridge_client_cpp::OP rosbridge_client_cpp::stringToOP(const std::string& op)
{
    using namespace rosbridge_client_cpp;
    if      (op == "advertise_service")     return OP::ServiceAdvertiseOp;
    else if (op == "unadvertise_service")   return OP::ServiceUnadvertiseOp;
    else if (op == "call_service")          return OP::ServiceCallOp;
    else if (op == "service_response")      return OP::ServiceResponseOp;
    else if (op == "advertise")             return OP::TopicAdvertiseOp;
    else if (op == "unadvertise")           return OP::TopicUnadvertiseOp;
    else if (op == "publish")               return OP::TopicPublishOp;
    else if (op == "subscribe")             return OP::TopicSubscribeOp;
    else if (op == "unsubscribe")           return OP::TopicUnsubscribeOp;
    else return OP::InvalidOp;
}

bool rosbridge_client_cpp::isTopicOP(rosbridge_client_cpp::OP op)
{
    using namespace rosbridge_client_cpp;
    static const std::set<OP> topic_ops = {
        OP::TopicAdvertiseOp,
        OP::TopicUnadvertiseOp,
        OP::TopicSubscribeOp,
        OP::TopicUnsubscribeOp,
        OP::TopicPublishOp
    };
    return topic_ops.count(op) > 0;
}

bool rosbridge_client_cpp::isServiceOP(rosbridge_client_cpp::OP op)
{
    using namespace rosbridge_client_cpp;
    static const std::set<rosbridge_client_cpp::OP> service_ops = {
        OP::ServiceAdvertiseOp,
        OP::ServiceUnadvertiseOp,
        OP::ServiceCallOp,
        OP::ServiceResponseOp,
    };
    return service_ops.count(op) > 0;
}

std::string std::to_string(rosbridge_client_cpp::OP op)
{
    using namespace rosbridge_client_cpp;
    switch (op)
    {
    case OP::ServiceAdvertiseOp:    return "advertise_service";
    case OP::ServiceUnadvertiseOp:  return "unadvertise_service";
    case OP::ServiceCallOp:         return "call_service";
    case OP::ServiceResponseOp:     return "service_response";
    case OP::TopicAdvertiseOp:      return "advertise";
    case OP::TopicUnadvertiseOp:    return "unadvertise";
    case OP::TopicSubscribeOp:      return "subscribe";
    case OP::TopicUnsubscribeOp:    return "unsubscribe";
    case OP::TopicPublishOp:        return "publish";
    case OP::InvalidOp:             return "_"; // for dummy messages
    }
    std::string e = std::string(__PRETTY_FUNCTION__) + " Invalid OP.";
    throw std::runtime_error(e);
}
