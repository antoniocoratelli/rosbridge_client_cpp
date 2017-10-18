/*
 * Copyright (C) 2017 Antonio Coratelli
 *
 
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
#include <gtest/gtest.h>
#include <rosbridge_client_cpp/queues.h>

using namespace rosbridge_client_cpp;

TEST(unit_test_queues, TalkerQueue)
{
    const int queue_size = 5;
    const int vector_size = 20;
    Serializer head(OP::TopicAdvertiseOp, "my_id");
    TalkerQueue queue(queue_size, head);
    std::vector<Serializer> serializers;

    ASSERT_GE(vector_size, queue_size);

    // filling the vector with data
    for (int i = 0; i < vector_size; i++)
    { Serializer msg(OP::TopicPublishOp, "my_id_" + std::to_string(i)); serializers.push_back(msg); }

    // pushing messages to the queue until we have space
    for (int i = 0; i < queue_size; i++)
    { queue.push(serializers[i]); }

    // verifying that the first message popped from the queue is the head
    // only before the first pop, or before the reset() call
    EXPECT_EQ(queue.next(), head); queue.pop();
    EXPECT_NE(queue.next(), head); queue.reset();
    EXPECT_EQ(queue.next(), head); queue.pop();

    // verifying that the messages in the queue after the head follow the insertion order
    for (int i = 0; i < queue_size; i++)
    { EXPECT_EQ(queue.next(), serializers[i]); queue.pop(); }

    // cleaning up the queue
    while(not queue.empty())
    { queue.pop(); }

    // pushing all the messages to the queue
    for (int i = 0; i < vector_size; i++)
    { queue.push(serializers[i]); }

    // verifying that only the last messages remained
    for (int i = vector_size - queue_size; i < vector_size; i++)
    { EXPECT_EQ(queue.next(), serializers[i]); queue.pop(); }
}

TEST(unit_test_queues, ListenerQueue)
{
    const int queue_size = 5;
    const int vector_size = 20;
    ListenerQueue queue(queue_size);
    std::vector<Unserializer> unserializers;

    // filling the vector with data
    for (int i = 0; i < vector_size; i++)
    { Unserializer msg("{ \"id\" : \"" + std::to_string(i) + "\" }"); unserializers.push_back(msg); }

    // pushing messages to the queue until we have space
    for (int i = 0; i < queue_size; i++)
    { queue.push(unserializers[i]); }

    // verifying that the messages in the queue follow the insertion order
    for (int i = 0; i < queue_size; i++)
    { EXPECT_EQ(queue.next(), unserializers[i]); queue.pop(); }

    // cleaning up the queue
    while(not queue.empty())
    { queue.pop(); }

    // pushing all the messages to the queue
    for (int i = 0; i < vector_size; i++)
    { queue.push(unserializers[i]); }

    // verifying that only the last messages remained
    for (int i = vector_size - queue_size; i < vector_size; i++)
    { EXPECT_EQ(queue.next(), unserializers[i]); queue.pop(); }

    // cleaning up the queue
    while(not queue.empty())
    { queue.pop(); }

    // pushing all the messages to the queue
    for (int i = 0; i < vector_size; i++)
    { queue.push(unserializers[i]); }

    // testing that retrieval by id works properly
    EXPECT_TRUE(queue.nextById("17").has_value());
    EXPECT_EQ(queue.nextById("17").value(), unserializers[17]);
    EXPECT_NO_THROW(queue.popById("17"));
    EXPECT_FALSE(queue.nextById("17").has_value());
}
