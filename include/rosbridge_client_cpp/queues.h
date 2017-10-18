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
#ifndef ROSBRIDGE_CLIENT_CPP_QUEUES_H
#define ROSBRIDGE_CLIENT_CPP_QUEUES_H

#include <atomic>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include "serializer.h"
#include "unserializer.h"
#include "vendor/optional.h"

namespace rosbridge_client_cpp
{

/**
 * The TalkerQueue class is used by a Publisher/Subscriber/... to store all the messages that
 * should be sent through RosbridgeClient.
 * The TalkerQueue contains a special "head" message (that can be for example the Advertise or Subscribe message)
 * which should be sent to RosbridgeServer every time a new connection is started.
 * This head message allows the Publisher/Subscriber/..(s) to re-declare themselves to the RosbridgeServer
 * after the connection is lost and obtained again.
 *
 * @attention The ownership of the TalkerQueue is formally shared between RosbridgeClient and the
 *            Publisher/Subscriber/... that instanciated the TalkerQueue.
 * @attention TalkerQueue is thread-safe in the case it is used by one consumer only and one producer only.
 */
class TalkerQueue
{
public:
    typedef std::shared_ptr<TalkerQueue> shared_ptr;

    TalkerQueue(int queue_size_, Serializer head_msg_);

    ~TalkerQueue();

    /// Adds a message to the queue.
    void push(Serializer msg);

    /// Returns the next message in the queue.
    Serializer next();

    /// Removes the first message from the queue.
    void pop();

    /// Returns true iff the queue has no more messages to send.
    bool empty();

    /// Marks the head message as not sent again, so it will be returned on the next call of `next()` method.
    void reset();

    /// Maks the queue as orphan, so it can be deallocated by RosbridgeClient
    inline void abandon() { this->orphan.store(true); }

    inline bool isOrphan() const { return this->orphan.load(); }

private:
    std::mutex mutex;
    int q_size;
    Serializer head_msg;
    bool head_popped = false;
    std::queue<Serializer> q;
    std::atomic_bool orphan{false};
};

/**
 * The ListenerQueue class is used by a Subscriber/ServiceServer/... to receive all the messages that
 * are received through RosbridgeClient.
 * @attention ListenerQueue is thread-safe in the case it is used by one consumer only and one producer only.
 */
class ListenerQueue
{
public:
    typedef std::shared_ptr<ListenerQueue> shared_ptr;

    explicit ListenerQueue(int queue_size_);

    ~ListenerQueue();

    /// Adds a message to the queue.
    void push(Unserializer message);

    /// Returns the next message in the queue.
    Unserializer next();

    /// Returns the element contained in the queue with requested ID, if it exists.
    std::optional<Unserializer> nextById(std::string id);

    /// Removes the first message from the queue.
    void pop();

    /// Removes the message from the queue identified by the requested ID.
    void popById(std::string id);

    /// Returns true iff the queue has no more messages to send.
    bool empty();

    /// Maks the queue as orphan, so it can be deallocated by RosbridgeClient
    inline void abandon() { this->orphan.store(true); }

    inline bool isOrphan() const { return this->orphan.load(); }

private:
    std::mutex mutex;
    int q_size;
    std::list<Unserializer> q;
    std::atomic_bool orphan{false};
};

} // namespace rosbridge_client_cpp

#endif // ROSBRIDGE_CLIENT_CPP_QUEUES_H
