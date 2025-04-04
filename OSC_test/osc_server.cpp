#include <lo/lo.h>
#include <iostream>
#include <thread>
#include <chrono>
// Callback function for received messages
int message_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
    std::cout << "Received message on " << path << ": " << argv[0]->f << argv[1]->f<< argv[2]->f<< std::endl;
    return 0;
}


int foo_handler(const char *path, const char *types, lo_arg ** argv,
    int argc, lo_message data, void *user_data)
{
/* example showing pulling the argument values out of the argv array */
printf("foo: %s <-i:%d\n\n", path, argv[0]->i);
fflush(stdout);

return 0;
}

int main() {
    // Create an OSC server listening on port 5005
    lo_server_thread server = lo_server_thread_new("5005", nullptr);

    if (!server) {
        std::cerr << "Failed to create OSC server!" << std::endl;
        return 1;
    }

    // Map the "/randomNum" address to the message handler
    lo_server_thread_add_method(server, "/objid", "i", foo_handler, nullptr);
    lo_server_thread_add_method(server, "/coordinateX/coordinateY/coordinateZ", "fff", message_handler, nullptr);
    //lo_server_thread_add_method(server, "/coordinateY", "f", message_handler, nullptr);
    //lo_server_thread_add_method(server, "/coordinateZ", "f", message_handler, nullptr);

    // Start the server
    lo_server_thread_start(server);
    std::cout << "Server listening on port 5005..." << std::endl;

    // Keep the server running
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
