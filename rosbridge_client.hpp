#ifndef ROSBRIDGE_CLIENT_HPP_6a8921f3bad0d781c5c70260def08b02a66ab6f1
#define ROSBRIDGE_CLIENT_HPP_6a8921f3bad0d781c5c70260def08b02a66ab6f1

#include <memory>
#include <optional>
#include <string>

#include <cpprest/json.h>

namespace rosbridge_client {

static_assert(__cplusplus >= 201703L, "C++17 or greater is required");

class Node;
std::shared_ptr<Node> initialize();

class UuidGenerator;

class Publisher {
public:
    Publisher(
            std::shared_ptr<Node> node,
            std::string topic,
            std::string message_type);

    ~Publisher();

    Publisher(Publisher const&) = delete;

    Publisher& operator=(Publisher const&) = delete;

    Publisher(Publisher&&) = default;

    Publisher& operator=(Publisher&&) = default;

    [[nodiscard]]
    std::string const& topic() const {
        return m_topic;
    }

    [[nodiscard]]
    std::string const& message_type() const {
        return m_message_type;
    }

    void send(web::json::value msg);

private:
    std::shared_ptr<Node> m_node;
    std::string m_id;
    std::string m_topic;
    std::string m_message_type;
};

class Subscriber {
public:
    Subscriber(
            std::shared_ptr<Node> node,
            std::string topic,
            std::string message_type);

    ~Subscriber();

    Subscriber(Subscriber const&) = delete;

    Subscriber& operator=(Subscriber const&) = delete;

    Subscriber(Subscriber&&) = default;

    Subscriber& operator=(Subscriber&&) = default;

    [[nodiscard]]
    std::string const& topic() const {
        return m_topic;
    }

    [[nodiscard]]
    std::string const& message_type() const {
        return m_message_type;
    }

private:
    std::shared_ptr<Node> m_node;
    std::string m_id;
    std::string m_topic;
    std::string m_message_type;
};

} // namespace rosbridge_client

#endif // ROSBRIDGE_CLIENT_HPP_6a8921f3bad0d781c5c70260def08b02a66ab6f1