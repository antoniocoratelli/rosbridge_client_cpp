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
#include <rosbridge_client_cpp/utils.h>

namespace rosbridge_client_cpp
{

Pinger::Pinger(std::string uri_): uri(uri_)
{
    RBC_LOG_INFO("INIT");
    curl_global_init(CURL_GLOBAL_ALL);
    this->handle = curl_easy_init();
    RBC_LOG_INFO("DONE");
}

bool Pinger::ping(int timeout_ms)
{
    char errorBuf[CURL_ERROR_SIZE];
    curl_easy_setopt(this->handle, CURLOPT_URL, this->uri.c_str());
    curl_easy_setopt(this->handle, CURLOPT_ERRORBUFFER, errorBuf);
    curl_easy_setopt(this->handle, CURLOPT_NOBODY, 1);
    curl_easy_setopt(this->handle, CURLOPT_TIMEOUT_MS, timeout_ms);
    CURLcode result = curl_easy_perform(this->handle);
    if (result != 0)
        RBC_LOG_WARN("PING ERROR");
    return result == 0;
}

Pinger::~Pinger()
{
    RBC_LOG_INFO("INIT");
    curl_easy_cleanup(this->handle);
    this->handle = nullptr;
    RBC_LOG_INFO("DONE");
}

Timer::Timer(): _execute(false)
{
    RBC_LOG_INFO("INIT");
    RBC_LOG_INFO("DONE");
}

Timer::~Timer()
{
    RBC_LOG_INFO("INIT");
    if (_execute.load(std::memory_order_acquire)) stop();
    RBC_LOG_INFO("DONE");
}

void Timer::stop()
{
    _execute.store(false, std::memory_order_release);
    if (_thd.joinable()) _thd.join();
}

void Timer::start(int interval_ms, std::function<void(void)> func)
{
    if (_execute.load(std::memory_order_acquire)) stop();

    _execute.store(true, std::memory_order_release);

    _thd = std::thread([this, interval_ms, func]()
    {
        while (_execute.load(std::memory_order_acquire))
        {
            func();
            auto ms = std::chrono::milliseconds(interval_ms);
            std::this_thread::sleep_for(ms);
        }
    });
}

bool Timer::is_running() const noexcept
{
    return (_execute.load(std::memory_order_acquire) and _thd.joinable());
}

} // namespace rosbridge_client_cpp
