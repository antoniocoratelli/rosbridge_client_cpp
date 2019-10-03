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

#include <atomic>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <tuple>
#include <vector>
#include<random>

#include <curl/curl.h>

#include "vendor/picojson.h"

#define RBC_COLOR_NORMAL "\033[0m"
#define RBC_COLOR_BLU    "\033[0;34m"
#define RBC_COLOR_GRN    "\033[0;32m"
#define RBC_COLOR_YEL    "\033[0;33m"
#define RBC_COLOR_RED    "\033[0;31m"

#define RBC_LOG_MSG(x) rosbridge_client_cpp::timestamp() << "] " << __PRETTY_FUNCTION__ << " " << x << RBC_COLOR_NORMAL
#define RBC_LOG_DEBUG(x) std::cout << RBC_COLOR_GRN << "[RBC_DEBUG::" << RBC_LOG_MSG(x) << std::endl
#define RBC_LOG_INFO(x)  std::cout << RBC_COLOR_BLU << "[RBC_INFO::"  << RBC_LOG_MSG(x) << std::endl
#define RBC_LOG_WARN(x)  std::cout << RBC_COLOR_YEL << "[RBC_WARN::"  << RBC_LOG_MSG(x) << std::endl
#define RBC_LOG_ERROR(x) std::cout << RBC_COLOR_RED << "[RBC_ERROR::" << RBC_LOG_MSG(x) << std::endl

namespace rosbridge_client_cpp
{

/**
 * Custom Types
 ** @{ */
typedef std::function<void(const picojson::object&)> TopicCallback;
typedef std::function<picojson::object(const picojson::object&)> ServiceServerCallback;
typedef std::function<void(const picojson::object&)> ServiceClientCallback;
typedef std::pair<std::string, std::string> key_value_t;
typedef std::vector<key_value_t> keys_values_t;
/** @} */

/**
 * Pinger provides a very simple interface to ping verify if a URI is reachable.
 * Pinger::ping() method returns true in case of ping succeded, false in case of ping failed.
 */
class Pinger
{
public:
    Pinger(std::string uri_);
    ~Pinger();
    bool ping(int timeout_ms = 500);
private:
    CURL * handle = nullptr;
    std::string uri;
};

/**
 * Timer provides a RAII interface for periodic callbacks.
 */
class Timer
{
public:
    typedef std::unique_ptr<Timer> uptr_t;
    Timer();
    ~Timer();
    void stop();
    void start(int interval, std::function<void(void)> func);
    bool is_running() const noexcept;
private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

/**
 * Returns a string with the number of nanoseconds since UNIX epoch.
 */
inline std::string timestamp()
{
    auto t = std::chrono::system_clock::now().time_since_epoch();
    auto m = std::chrono::duration_cast<std::chrono::nanoseconds>(t);
    return std::to_string(m.count());
}

/**
 * Returns a random string of desired length generated using a custom alphabet.
 */
inline std::string random_string(size_t length, std::string chars = "0123456789abcdefghijklmnopqrstuvwxyz")
{
    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, chars.size() - 1);
    std::string s; s.reserve(length);
    while(length--) s += chars[pick(rg)];
    return s;
}

/**
 * Returns a string that has a reasonably high probability of being different at each call,
 * even from different machines.
 */
inline std::string unique(size_t random_size = 4)
{
    return timestamp() + "-" + random_string(random_size);
}

/**
 * Converts a vector of key-value pairs to json object.
 * Useful to obtain a json object in code using initializer lists, with a syntax that is actually
 * very similar to the JSON syntax.
 * Example:
 * @code
 *   to_json({ {"key_1", "value-vw2L45Ei"}, {"key_2", "value-bqxALKiz"} });
 * @endcode
 */
inline picojson::object to_json(keys_values_t keys_values)
{
    picojson::object json;
    for (auto& key_value : keys_values) json[key_value.first] = picojson::value(key_value.second);
    return json;
}

/**
 * Converts a json object to string.
 */
inline std::string to_string(picojson::object json)
{
    return picojson::value(json).serialize();
}

} // namespace rosbridge_client_cpp

namespace std
{
inline std::string to_string(picojson::object j)
{
    return rosbridge_client_cpp::to_string(j);
}
} // namespace std