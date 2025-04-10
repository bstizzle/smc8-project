#include <lo/lo.h> //installed via sudo apt install liblo-dev
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

int main() {

    // Initialize random seed
    std::srand(std::time(nullptr));

    // Create an OSC client that sends to localhost on port 5005
    lo_address target = lo_address_new("192.168.1.100", "5005");

    if (!target) {
        std::cerr << "Failed to create OSC client!" << std::endl;
        return 1;
    }

    for (int i = 0; i < 1000; i++) {
        float random_value = static_cast<float>(std::rand()) / RAND_MAX * 200 + 200;
        lo_send(target, "/randomNum", "f", random_value);  // Send float data
        std::cout << "Sent: " << random_value << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
