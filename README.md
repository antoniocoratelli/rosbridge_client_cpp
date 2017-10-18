# rosbridge_client_cpp

Implementation of a C++11 [rosbridge client][wiki_rosbridge] for rosbridge protocol v2.0.

The library was designed to:

- Be robust to communication failures;

- Handle transparently reconnections in case of temporary interruptions of communication;

- Have an interface as similar as possible to the ROS standard one.

## Usage

In order to declare `Publisher`s, `Subscriber`s, `ServiceClient`s, and `ServiceServer`s, you need to create an
instance of the `RosbridgeClient`:

```
#include <rosbridge_client_cpp/rosbridge.h>

auto on_connection = [](){std::cout << "RosbridgeClient connected" << std::endl};
auto on_disconnection = [](){std::cout << "RosbridgeClient disconnected" << std::endl};
RosbridgeClient rb("localhost", 9090, on_connection, on_disconnection);
```

#### Publisher

```
Publisher my_pub(rb, "/my/topic", "std_msgs/String", 20);
picojson::object json;
json["data"] = picojson::value("Hello There!");
my_pub.publish<picojson::object>(json);
```

#### Subscriber

```
auto my_callback = [](const picojson::object& json){std::cout << std::to_string(json) << std::endl;};
Subscriber my_sub(rb, "/my/topic", "std_msgs/String", my_callback, 5);
```

#### ServiceClient

```
ServiceClient my_client(rb, "/my/service");
picojson::object json;
json["data"] = picojson::value("Hello There!");
my_client.call(json, std::chrono::seconds(5));
```

#### ServiceServer

```
auto my_callback = [](const picojson::object& request) -> picojson::object
{
    picojson::object response;
    response["success"] = picojson::value(true);
    response["message"] = picojson::value("Hello There!");
    return response;
};
ServiceServer my_server("/my/service", "std_srvs/Trigger", my_callback);
```

## External dependencies

```
sudo apt-get install zlib1g-dev libssl-dev libcurl4-openssl-dev
```

## Build and Install

```
cd <path-to-repository>
mkdir build && cd build
cmake .. && make -j$(nproc)
sudo make install
```

## License

```
Copyright (C) 2017 Antonio Coratelli

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
```

## Acknowledgements

This library links statically:

- `std::optional` by *Andrzej Krzemienski*, released under *Boost Software License*;

- `asio` by *Christopher M. Kohlhoff*, released under *Boost Software License*;

- `websocketpp` by *Peter Thorson*, released under *BSD 3-Clause License*;

- `picojson` by *Cybozu Labs, Inc.* and *Kazuho Oku*, released under *BSD 2-Clause*.

## Possible improvements for the future:

- Use condition variables instead of loops with a fixed rate;

- Deallocate orphan `TalkerQueue`s and `ListenerQueue`s;

- Add message type verifications.


[wiki_rosbridge]: http://wiki.ros.org/rosbridge_suite
