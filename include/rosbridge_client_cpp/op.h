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
#ifndef ROSBRIDGE_CLIENT_CPP_OP_H
#define ROSBRIDGE_CLIENT_CPP_OP_H

#include <set>
#include <string>
#include <stdexcept>

namespace rosbridge_client_cpp
{

/**
 * Enum that defines all the possible operations rosbridge operations.
 */
enum OP
{
    ServiceAdvertiseOp,
    ServiceUnadvertiseOp,
    ServiceCallOp,
    ServiceResponseOp,
    TopicAdvertiseOp,
    TopicUnadvertiseOp,
    TopicSubscribeOp,
    TopicUnsubscribeOp,
    TopicPublishOp,
    InvalidOp ///< this is an addition used as a return value in stringToOP if the input string is not valid
};

/**
 * Converts a string to the corresponing OP enum
 */
OP stringToOP(const std::string& op);

/**
 * Returns true iff the OP is an operation on a Topic.
 */
bool isTopicOP(rosbridge_client_cpp::OP op);

/**
 * Returns true iff the OP is an operation on a Service.
 */
bool isServiceOP(rosbridge_client_cpp::OP op);

}

namespace std
{
/**
 * Extension to the std namespace to provide a `to_string` method for the `rosbridge_client_cpp::OP`
 */
std::string to_string(rosbridge_client_cpp::OP op);
}

#endif // ROSBRIDGE_CLIENT_CPP_OP_H
