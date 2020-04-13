#include "rosbridge_client.hpp"

int main() {
    auto node = rosbridge_client::initialize();
    return bool{node} ? 0 : 1;
}
