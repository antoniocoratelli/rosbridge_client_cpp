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
#include <rosbridge_client_cpp/queues.h>

using namespace rosbridge_client_cpp;

TalkerQueue::TalkerQueue(int queue_size_, Serializer head_msg_):
    q_size(queue_size_),
    head_msg(head_msg_)
{
    RBC_LOG_DEBUG("TalkerQueue{id=" << this << ", size=" << this->q_size << "}");
}

TalkerQueue::~TalkerQueue()
{
    RBC_LOG_DEBUG("~TalkerQueue{id=" << this << ", size=" << this->q_size << "}");
}

void TalkerQueue::push(Serializer msg)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->q.push(msg);
    while (this->q.size() > this->q_size) this->q.pop();
}

void TalkerQueue::pop()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->head_popped) this->q.pop(); else this->head_popped = true;
}

Serializer TalkerQueue::next()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return (this->head_popped) ? this->q.front() : this->head_msg;
}

bool TalkerQueue::empty()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->head_popped and this->q.empty();
}

void TalkerQueue::reset()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->head_popped = false;
}

ListenerQueue::ListenerQueue(int queue_size_):
    q_size(queue_size_)
{
    RBC_LOG_DEBUG("ListenerQueue{id=" << this << ", size=" << this->q_size << "}");
}

ListenerQueue::~ListenerQueue()
{
    RBC_LOG_DEBUG("~ListenerQueue{id=" << this << ", size=" << this->q_size << "}");
}

void ListenerQueue::push(Unserializer unserializer)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->q.push_back(unserializer);
    while (this->q.size() > this->q_size) this->q.pop_front();
}

Unserializer ListenerQueue::next()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->q.front();
}

void ListenerQueue::pop()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->q.pop_front();
}

bool ListenerQueue::empty()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->q.empty();
}

std::optional<Unserializer> ListenerQueue::nextById(std::string id)
{
    auto cond = [id](Unserializer& element){ return element.id() == id; };
    auto iter = std::find_if(this->q.begin(), this->q.end(), cond);
    if (iter == this->q.end()) return {};
    else return {*iter};
}

void ListenerQueue::popById(std::string id)
{
    auto cond = [id](Unserializer& element){ return element.id() == id; };
    auto iter = std::remove_if(this->q.begin(), this->q.end(), cond);
    this->q.erase(iter);
}
