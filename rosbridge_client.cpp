#include "rosbridge_client.hpp"

#include <chrono>
#include <iomanip>
#include <limits>
#include <mutex>
#include <random>
#include <sstream>

class rosbridge_client::UuidGenerator {
public:
    UuidGenerator():
        m_rnd{std::random_device{}()},
        m_uniform{0, std::numeric_limits<uint32_t>::max()} {
    }

    uint64_t operator()() {
        return (uint64_t{get_epoch_ms()} << 32U) + m_uniform(m_rnd);
    }

private:
    std::mt19937 m_rnd;
    std::uniform_int_distribution<uint32_t> m_uniform;

    [[nodiscard]]
    uint32_t get_epoch_ms() const {
        using std::chrono::duration_cast;
        using duration_type = std::chrono::duration<uint32_t, std::milli>;
        auto const now = std::chrono::system_clock::now().time_since_epoch();
        return duration_cast<duration_type>(now).count();
    }
};

class rosbridge_client::Node {
public:
    Node() = default;

    void send(web::json::value const& op) {
        std::lock_guard<std::mutex> guard{m_mutex};
    }

    std::optional<web::json::object> poll() {
        std::lock_guard<std::mutex> guard{m_mutex};
        return {};
    }

    std::string generate_id() {
        std::lock_guard<std::mutex> guard{m_mutex};
        std::stringstream s;
        auto const uuid = m_uuid_generator();
        s << std::hex << std::setw(sizeof(uuid)) << std::setfill('0') << uuid;
        return s.str();
    }

private:
    std::mutex m_mutex;
    rosbridge_client::UuidGenerator m_uuid_generator;
};

std::shared_ptr<rosbridge_client::Node> rosbridge_client::initialize() {
    return std::make_shared<rosbridge_client::Node>();
}

rosbridge_client::Publisher::Publisher(
        std::shared_ptr<Node> node,
        std::string topic,
        std::string message_type):
    m_node{std::move(node)},
    m_id{m_node->generate_id()},
    m_topic{std::move(topic)},
    m_message_type{std::move(message_type)} {
    auto op = web::json::value::object();
    op["op"] = web::json::value::string("advertise");
    op["id"] = web::json::value::string(m_id);
    op["topic"] = web::json::value::string(m_topic);
    op["type"] = web::json::value::string(m_message_type);
    m_node->send(op);
}

rosbridge_client::Publisher::~Publisher() {
    if (not bool{m_node}) return;
    auto op = web::json::value::object();
    op["op"] = web::json::value::string("unadvertise");
    op["id"] = web::json::value::string(m_id);
    op["topic"] = web::json::value::string(m_topic);
    m_node->send(op);
}

void rosbridge_client::Publisher::send(web::json::value msg) {
    auto op = web::json::value::object();
    op["op"] = web::json::value::string("publish");
    op["id"] = web::json::value::string(m_id);
    op["topic"] = web::json::value::string(m_topic);
    op["msg"] = std::move(msg);
    m_node->send(op);
}
