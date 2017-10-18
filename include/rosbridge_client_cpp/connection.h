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
#ifndef ROSBRIDGE_CLIENT_CPP_CONNECTION_H
#define ROSBRIDGE_CLIENT_CPP_CONNECTION_H

#include "utils.h"

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

namespace rosbridge_client_cpp
{

class Connection
{
public:

    /**
     * Initializes a robust Connection.
     * @param address_ Ip address or hostname of the RosbridgeServer.
     * @param port_ Port number of the RosbridgeServer.
     * @param max_ping_pong_seconds Maximum number of seconds allowed between a ping and a pong; in case of ping pong
     *                              timeout, the connection is dropped.
     */
    Connection(std::string address_, int port_, int max_ping_pong_seconds = 2):
        address(address_ + ":" + std::to_string(port_)),
        max_ping_pong_interval(max_ping_pong_seconds)
    {
        RBC_LOG_INFO("Hello!");
        std::thread(&Connection::runner, this).detach();
    }

    /**
     * Sends a messages through the robust Connection.
     */
    void send(const std::string& data)
    {
        if (not this->active())
            return;
        try  { this->message_sender(data); }
        catch (...) { RBC_LOG_ERROR("EXCEPTION WHILE SENDING"); }
    }

    ~Connection()
    {
        RBC_LOG_INFO("");
        this->shutting_down = true;
        RBC_LOG_INFO("Goodbye!");
    }

    std::function<bool(void)> active = []{return false;};

    /// Callbacks
    /// @{
    typedef std::function<void(void)> callback_t;
    typedef std::function<void(const std::string&)> callback_msg_t;
    callback_t on_connection = []{};
    callback_t on_disconnection = []{};
    callback_msg_t on_message_received = [](const std::string&){};
    /// @}

private:

    std::string address;
    std::atomic<bool> connection_active{false};
    std::atomic<bool> shutting_down{false};
    const std::chrono::seconds max_ping_pong_interval;

    callback_msg_t message_sender = [](const std::string&){};

    typedef websocketpp::client<websocketpp::config::asio_client> client;
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
    typedef websocketpp::config::asio_client::message_type message;
    typedef websocketpp::session::state::value connection_state;
    typedef websocketpp::frame::opcode::value opcode;

    std::string to_string(connection_state state)
    {
        if (state == connection_state::closed)     return "closed";
        if (state == connection_state::closing)    return "closing";
        if (state == connection_state::connecting) return "connecting";
        if (state == connection_state::open)       return "open";
        return "UNKNOWN";
    }

    void runner()
    {
        RBC_LOG_WARN("");
        while(not this->shutting_down)
        {
            try
            {
                client c;
                client::connection_ptr connection;
                this->message_sender = [&](const std::string& p){ c.send(connection, p, opcode::text); };
                this->connect(c, connection);
            }
            catch (const std::exception & e) { RBC_LOG_ERROR("std::" << e.what()); }
            catch (websocketpp::lib::error_code e) { RBC_LOG_ERROR("websocket::" << e.message()); }
            catch (...) { RBC_LOG_ERROR("Other exception"); }
            this->message_sender = [](const std::string&){};
            this->active = []{return false;};
        }
    }

    void connect(client& c, client::connection_ptr& connection)
    {
        c.set_message_handler([this](websocketpp::connection_hdl, message_ptr msg){
            this->on_message_received(msg->get_payload());
        });
        c.set_open_handler([this](websocketpp::connection_hdl){ this->on_connection(); });
        c.set_close_handler([this](websocketpp::connection_hdl){ this->on_disconnection(); });
        c.clear_access_channels(websocketpp::log::alevel::frame_header);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_close_handshake_timeout(500);
        c.set_open_handshake_timeout(1000);
        c.init_asio();

        RBC_LOG_INFO("trying to reconnect to " << this->address << " ...");
        websocketpp::lib::error_code ec;
        connection = c.get_connection("ws://" + this->address, ec);

        Timer timer_ping_send;
        Timer timer_pong_check;
        std::chrono::high_resolution_clock::time_point time_last_ping = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point time_last_pong = std::chrono::high_resolution_clock::now();

        auto handler_pong = [&](websocketpp::connection_hdl, std::string){
            time_last_pong = std::chrono::high_resolution_clock::now();
            RBC_LOG_DEBUG("pong");
        };
        auto handler_ping = [&](){
            try
            {
                c.ping(connection, "ping-"+unique());
                time_last_ping = std::chrono::high_resolution_clock::now();
                RBC_LOG_DEBUG("ping");
            }
            catch(...) {};
        };
        auto handler_ping_pong_check = [&](){
            try
            {
                if (time_last_ping - time_last_pong > max_ping_pong_interval)
                    c.close(connection, websocketpp::close::status::normal, "pong missed");
            }
            catch(...) {};
        };

        connection->set_pong_handler(handler_pong);
        timer_ping_send.start(1000, handler_ping);
        timer_pong_check.start(1000, handler_ping_pong_check);

        if (connection->get_state() == connection_state::connecting)
        {
            RBC_LOG_DEBUG("PRE_CONNECT");
            c.connect(connection);
            RBC_LOG_DEBUG("PRE_RUN");
            this->active = [&]{return connection->get_state() == connection_state::open;};
            c.run();
            this->on_disconnection();
            RBC_LOG_DEBUG("POST_RUN");
            this->message_sender = [](const std::string&){};
            this->active = []{return false;};
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

} // namespace rosbridge_client_cpp

#endif // ROSBRIDGE_CLIENT_CPP_OP_H
