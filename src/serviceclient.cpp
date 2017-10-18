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
#include <rosbridge_client_cpp/serviceclient.h>

namespace rosbridge_client_cpp
{

ServiceClient::ServiceClient(RosbridgeClient& rb, const std::string& service_, int queue_size_):
    service(service_),
    shutdown_requested(false)
{
    Serializer no_op(OP::InvalidOp);
    this->talker_queue.reset(new TalkerQueue(queue_size_, no_op));
    this->listener_queue.reset(new ListenerQueue(queue_size_));
    RBC_LOG_DEBUG("{"
        << "service=" << service << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    rb.attach(this->talker_queue);
    rb.attach(this->listener_queue, this->service);
}

ServiceClient::~ServiceClient()
{
    this->shutdown_requested.store(true);
    RBC_LOG_DEBUG("{"
        << "service=" << service << ", "
        << "talker_queue=" << this->talker_queue.get() << ", "
        << "listener_queue=" << this->listener_queue.get() << "}");
    this->talker_queue->abandon();
    this->listener_queue->abandon();
}

template<>
std::optional<picojson::object> ServiceClient::call(picojson::object input, std::chrono::seconds timeout)
{
    using std::chrono::milliseconds;
    using std::chrono::high_resolution_clock;

    SerializerServiceRequest request(this->service, input);

    const std::string HEAD = "ServiceCall[" + request.id() + "] " + this->service + " ";
    RBC_LOG_INFO(HEAD << "REQUEST" << std::to_string(input));
    this->talker_queue->push(request);

    high_resolution_clock::time_point start = high_resolution_clock::now();
    while(high_resolution_clock::now() - start < timeout)
    {
        std::this_thread::sleep_for(milliseconds(5));
        std::optional<Unserializer> u = this->listener_queue->nextById(request.id());
        if (u.has_value() and u.value().result())
        {
            picojson::object response = u.value().payload();
            RBC_LOG_INFO(HEAD << "RESPONSE" << std::to_string(response));
            return {response};
        }
    }
    RBC_LOG_INFO(HEAD << "TIMEOUT");
    return {};
}

template<>
std::optional<picojson::object> ServiceClient::call(keys_values_t input, std::chrono::seconds timeout)
{
    picojson::object json = to_json(input);
    return this->call(json, timeout);
}

}
